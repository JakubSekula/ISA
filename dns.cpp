/******************************************************************************
 * Project: Filtrující DNS resolver                                           *
 * Subject: ISA - Network Applications and Network Administration             *
 * Rok:     2020/2021                                                         *
 * Authors:                                                                   *
 *			Jakub Sekula  (xsekul01) - xsekul01@stud.fit.vutbr.cz             *
 ******************************************************************************/

// TODO: prepsat promenou test
// TODO: -s dns.google zatim nefunguje

/**
 * @file dns.cpp
 * @author Jakub Sekula( xsekul01 )
 * @date 10.10.2020
 * @brief DNS server
 */

#include "dns.hpp"
#include "ext/dnshdr.hpp"

using namespace std;

/**
 * IP version 
 */
int ver = 4;

/**
 * Port number
 */
int port = 53;

/**
 * Blacklisted domain file
 */
string file = "";

/**
 * DNS server name or ip
 */
string server = "";

/**
 * @brief Resolves ip of given server
 * @param server ip or server name
 * @return resolved ip
 */
string getDnsIp( const char* server, int ver ){
    struct addrinfo hints, *res;
    memset( &hints, 0, sizeof ( hints ) );

    if( ver == 4 ){
        hints.ai_family = AF_INET;
    } else {
        hints.ai_family = AF_INET6;
    }

    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_CANONNAME;
    
    if( getaddrinfo ( server, NULL, &hints, &res ) != 0 ){
        return( "" );
    }
    
    void* ptr;
    string addrstr;
    
    inet_ntop( res->ai_family, res->ai_addr->sa_data, (char*) addrstr.c_str(), 100 );

    if( ver == 4 ){
        ptr = &( ( struct sockaddr_in *) res->ai_addr )->sin_addr;
    } else {
        ptr = &( ( struct sockaddr_in6 *) res->ai_addr )->sin6_addr;
    }
    inet_ntop (res->ai_family, ptr, (char*) addrstr.c_str(), 100);
    return addrstr.c_str();
}

/**
 * @brief Checks if parametr is numerical
 * @param s program argument
 * @return true, or false when argument is not numerical
 */
bool isNumber( string s )
{
    for( unsigned int i = 0; i < s.length(); i++ )
        if( isdigit( s[ i ] ) == false )
            return false;
 
    return true;
}

/**
 * @brief Checks if given file exists
 * @param file file
 */
void fileExists( char* file ){
    string filepath = file;
    ifstream ifile( filepath );
    if( !ifile ){
        fprintf( stderr, "File does not exists\n" );
        exit( 9 );
    }
}

/**
 * @brief Loads command line arguments
 * @param -p port number ( optional )
 * @param -s IP adress or domain name of DNS server
 * @param -f File with unwanted domains
 */
void getArguments( int argc, char** argv ){
    int arg;
    string temp = "";
    while( ( arg = getopt( argc, argv, "s:p:f:" ) ) != -1 ){
        switch( arg ){
            case 's':
                server = optarg;
                temp = getDnsIp( server.c_str(), 6 );
                if( temp != "" ){
                    ver = 6;
                    server = temp;
                    break;
                }
                temp = getDnsIp( server.c_str(), 4 );
                if( temp != "" ){
                    ver = 4;
                    server = temp;
                    break;
                }
                
                fprintf( stderr, "Value given by -s parameter could not be converted to ip address\n" );
                exit( 10 );
                
                break;
            case 'p':
                if( !isNumber( optarg ) ){
                    fprintf( stderr, "Port must be an integer in range of 1024 to 65535\n" );
                    exit( 10 );    
                }
                port = atoi( optarg );
                if( port < 1024 || port > 65535 ){
                    fprintf( stderr, "Port must be an integer in range of 1024 to 65535\n" );
                    exit( 10 );
                }
                break;
            case 'f':
                fileExists( optarg );
                file = optarg;
                break;
            default:
                fprintf( stderr, "Uknown argument\n" );
                exit( 10 );
        }
    }

    if( server == "" ){
        fprintf( stderr, "Server argument must be passed\n" );
        exit( 10 );
    } else if ( file == "" ){
        fprintf( stderr, "File with blacklisted domain names must be passed\n" );
        exit( 10 );
    }

}

/**
 * @brief Splits a string by a char
 * @param s string
 * @param c splitter
 * @return divided string as vector
 */
vector<string> explode( string Elem, char find ){
	
	vector<string> exploded;
	string temp = "";

    for( char c : Elem ){
        if( c == find ){
            exploded.push_back( temp );
            temp = "";
        } else {
            temp = temp + c;
        }
    }

    exploded.push_back( temp );

    return exploded;

}

/**
 * @brief Searches given file for domain or subdomain
 * @param addr web addres
 * @return true or false when addr is located on the blacklist
 */
int searchFile( string addr ){
    
    vector<string> domain = explode( addr, '.' );

    ifstream infile( file );
    string line;

    while( getline( infile, line ) ){
        if( line[ 0 ] == '#' || line.empty() ){
            // skip line
        } else {
            // parse line
            vector<string> parsedLine = explode( line, '.' );

            if( domain.size() < parsedLine.size() ){
                continue;
            } else {
                int i = domain.size() - 1;
                int p = parsedLine.size() - 1;
                while( p >= 0 ){
                    if( domain[ i ].compare( parsedLine[ p ] ) == 0 ){
                        i--;
                        p--;
                        // there is nothing left on a line
                        if( p < 0 ){
                            return false;
                            break;
                        }
                    } else {
                        break;
                    }
                }
            }
        }
    }
    return true;

}

/**
 * @brief Send DNS response to client with rcode and qr set acording to error
 * @param socketfd communication socket
 * @param pd DNS packet structure
 * @param buffer packet
 * @param size packet size
 * @param cliaddr6 address structure
 * @param rcode error code
 * @param qr response or query flag
 */
void sendDnsError( int socketfd, dnshdr* pd, int size, sockaddr_in6 cliaddr6, int rcode, int qr ){
    
    pd->rcode = rcode;
    pd->qr = qr;
    
    sendto( socketfd, pd, size, 0, ( const struct sockaddr * ) &cliaddr6, sizeof( cliaddr6 ) );

    close( socketfd );
    exit( 0 );

}

/**
 * @brief DNS server
 * @param -p port number ( optional )
 * @param -s IP adress or domain name of DNS server
 * @param -f File with unwanted domains
 * @return 0 or int > 0 when error occurs
 */
int main( int argc, char **argv ){

    // Elimination of child processes
    signal( SIGCHLD, SIG_IGN );
    
    getArguments( argc, argv );

    int socketfd;
    struct sockaddr_in dns4;
    struct sockaddr_in6 my_addr6, cliaddr6, dns6;
    char buffer[ 1024 ] = {0};

    socketfd = socket( AF_INET6, SOCK_DGRAM, 0 );

    if( ( socketfd ) < 0 ){
        fprintf( stderr, "Socket could not be created\n" );
        exit( 12 );
    }
    
    memset( &my_addr6, 0, sizeof( my_addr6 ) );
    my_addr6.sin6_family = AF_INET6;
    my_addr6.sin6_addr = IN6ADDR_ANY_INIT;
    my_addr6.sin6_port = htons( port );

    int bindVal;

    bindVal =  bind( socketfd, ( struct sockaddr *) &my_addr6, sizeof( my_addr6 ) );

    if( bindVal < 0 ){
        fprintf( stderr, "Bind error\n" );
        exit( 13 );
    }

    // main server cycle
    while( 1 ){

        socklen_t clientaddrlen;

        memset( &cliaddr6, 0, sizeof( cliaddr6 ) );
        clientaddrlen = sizeof( cliaddr6 );
    
        int clientpacket;
        clientpacket = recvfrom( socketfd, buffer, 1024, 0, ( struct sockaddr *) &cliaddr6, &clientaddrlen );   
        
        int recvlen = clientpacket;

        int pid;

        // child process
        if( ( pid = fork() ) == 0 ){

            dnshdr* pd = ( dnshdr* ) buffer;

            // 13 is the byte right after header which says number of chars following
            int i = 13;
            int last = i;
            bool format = false;
            string domain;
            // number of chars
            int length = buffer[ i - 1 ];

            // cycle through QNAME
            while( length != 0 ){
                while( i < last + length ){
                    domain = domain + buffer[ i ];
                    i++;
                }
                length = buffer[ i ];
                i++;
                last = i;
                if( length != 0 ){
                    format = true;
                    domain = domain + '.';
                }
            }

            // TODO: je to chyba ?
            if( format == false ){
                sendDnsError( socketfd, pd, recvlen, cliaddr6, 1, 1 );
            }

            // transforming 8bit buffer into 16 bit for getting QTYPE
            uint16_t* bufferfield = (uint16_t*) buffer;

            if( ( i % 2 ) != 0 ){
                // kvuli deleni 2
                i--;
                // in case of wrong packet size
                bufferfield = (uint16_t*)( ( (char*) bufferfield ) + 1 );
            }

            // 8bits -> 16 bits
            clientpacket = clientpacket / 2;

            int idx = i / 2;
            uint16_t type = bufferfield[ idx ];

            if( ntohs( type ) != 1 ){
                sendDnsError( socketfd, pd, recvlen, cliaddr6, 4, 1 );
            }

            clientpacket = clientpacket * 2;

            if( searchFile( domain ) == 0 ){
                sendDnsError( socketfd, pd, recvlen, cliaddr6, 5, 1 );
            } else {

                int newsocket;
                if( ver == 4 ){
                    newsocket = socket( AF_INET, SOCK_DGRAM, 0 );
                } else {
                    newsocket = socket( AF_INET6, SOCK_DGRAM, 0 );
                }

                // new socket for DNS server communication               
                if( newsocket < 0 ){
                    fprintf( stderr, "Socket could not be created\n" );
                    exit( 12 );
                }

                if( ver == 4 ){
                    memset( &dns4, 0, sizeof( dns4 ) );
                    dns4.sin_family = AF_INET;
                    dns4.sin_addr.s_addr = inet_addr( server.c_str() );
                    dns4.sin_port = htons( 53 );
                    sendto( newsocket, buffer, recvlen, 0, ( const struct sockaddr * ) &dns4, sizeof( dns4 ) );
                } else {
                    unsigned char buf[ sizeof( struct in6_addr ) ];
                    memset( &dns6, 0, sizeof( dns6 ) );
                    dns6.sin6_family = AF_INET6;

                    inet_pton( AF_INET6, server.c_str(), buf );
                    memcpy( &dns6.sin6_addr, buf, sizeof( struct in6_addr ) );

                    dns6.sin6_port = htons( 53 );

                    sendto( newsocket, buffer, recvlen, 0, ( const struct sockaddr * ) &dns6, sizeof( dns6 ) );
                }

                
                socklen_t testaddrlen;
                int toclient;
                
                testaddrlen = sizeof( dns6 );
                toclient = recvfrom( newsocket, (char *)buffer, 1024, 0, ( struct sockaddr *) &dns6, &testaddrlen );
                sendto( socketfd, buffer, toclient, 0, ( const struct sockaddr * ) &cliaddr6, sizeof( cliaddr6 ) );

            }

            close( socketfd );
            exit( 0 );

        }
    }
}
/******************************************************************************
 * Project: Filtrující DNS resolver                                           *
 * Subject: ISA - Network Applications and Network Administration             *
 * Rok:     2020/2021                                                         *
 * Authors:                                                                   *
 *			Jakub Sekula  (xsekul01) - xsekul01@stud.fit.vutbr.cz             *
 ******************************************************************************/

/**
 * @file dns.cpp
 * @author Jakub Sekula( xsekul01 )
 * @date 10.10.2020
 * @brief DNS server
 */

#include "dns.h"

using namespace std;

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
string getDnsIp( const char* server ){
    struct addrinfo hints, *res;
    memset( &hints, 0, sizeof ( hints ) );

    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_CANONNAME;
    
    if( getaddrinfo ( server, NULL, &hints, &res ) != 0 ){
        fprintf( stderr, "Given servername or ip address by -s argument is not resolvable\n" );
        exit( 11 );
    }
    
    void* ptr;
    string addrstr;
    
    inet_ntop( res->ai_family, res->ai_addr->sa_data, (char*) addrstr.c_str(), 100 );

    ptr = &( ( struct sockaddr_in * ) res->ai_addr )->sin_addr;
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

    while( ( arg = getopt( argc, argv, "s:p:f:" ) ) != -1 ){
        switch( arg ){
            case 's':
                server = optarg;
                server = getDnsIp( server.c_str() );
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
const vector<string> explode( const string& s, const char& c ){
	
    string buff{""};
	vector<string> v;
	
	for( auto n:s ){
		if( n != c ){
            buff += n;
        } else if( n == c && buff != "" ){
            v.push_back( buff ); 
            buff = ""; 
        }
	}
	if( buff != "" ){
        v.push_back( buff );
    }

	return v;
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
 * @param cliaddr address structure
 * @param rcode error code
 * @param qr response or query flag
 */
void sendDnsError( int socketfd, dnshdr* pd, char* buffer, int size, sockaddr_in cliaddr, int rcode, int qr ){
    
    pd->rcode = rcode;
    pd->qr = qr;
                
    sendto( socketfd, buffer, size, 0, ( const struct sockaddr * ) &cliaddr, sizeof( cliaddr ) );

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
    
    getArguments( argc, argv );

    int socketfd;
    struct sockaddr_in my_addr, cliaddr, test;
    char buffer[ 1024 ] = {0};

    if( ( socketfd = socket( AF_INET, SOCK_DGRAM, 0 ) ) < 0 ){
        fprintf( stderr, "Socket could not be created\n" );
        exit( 12 );
    }
    
    memset( &my_addr, 0, sizeof( my_addr ) );
        
    my_addr.sin_family = AF_INET;
    my_addr.sin_addr.s_addr = INADDR_ANY;
    my_addr.sin_port = htons( port );

    if( bind( socketfd, ( struct sockaddr *) &my_addr, sizeof( my_addr ) ) < 0 ){
        fprintf( stderr, "Bind error\n" );
        exit( 13 );
    }
    
    // main server cycle
    while( 1 ){

        memset( &cliaddr, 0, sizeof( cliaddr ) );
        //int len = sizeof( cliaddr );
        socklen_t clientaddrlen = sizeof(cliaddr);

        int clientpacket = recvfrom( socketfd, buffer, 1024, 0, ( struct sockaddr *) &cliaddr, &clientaddrlen );

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

            if( format == false ){
                sendDnsError( socketfd, pd, buffer, clientpacket, cliaddr, 1, 1 );
            }

            // transforming 8bit buffer into 16 bit for getting QTYPE
            uint16_t* bufferfield = (uint16_t*) buffer;
            
            if( clientpacket % 2 != 0 ){
                // in case of wrong packet size
                bufferfield = (uint16_t*)( ( (char*) bufferfield ) + 1 );
            }

            // 8bits -> 16 bits
            clientpacket = clientpacket / 2;

            uint16_t type = bufferfield[ clientpacket - 2 ];

            if( ntohs( type ) != 1 ){
                sendDnsError( socketfd, pd, buffer, clientpacket, cliaddr, 4, 1 );
            }

            clientpacket = clientpacket * 2;

            if( searchFile( domain ) == 0 ){
                sendDnsError( socketfd, pd, buffer, clientpacket, cliaddr, 5, 1 );
            } else {

                int newsocket;

                // new socket for DNS server communication               
                if( ( newsocket = socket( AF_INET, SOCK_DGRAM, 0 ) ) < 0 ){
                    fprintf( stderr, "Socket could not be created\n" );
                    exit( 12 );
                }

                memset( &test, 0, sizeof( test ) );

                test.sin_family = AF_INET;
                test.sin_addr.s_addr = inet_addr( server.c_str() );
                test.sin_port = htons( 53 );
                
                sendto( newsocket, buffer, recvlen, 0, ( const struct sockaddr * ) &test, sizeof( test ) );
                
                //len = sizeof( test );
                socklen_t testaddrlen = sizeof(test);
                int toclient = recvfrom( newsocket, (char *)buffer, 1024, 0, ( struct sockaddr *) &test, &testaddrlen );

                sendto( socketfd, buffer, toclient, 0, ( const struct sockaddr * ) &cliaddr, sizeof( cliaddr ) );
            }

            close( socketfd );
            exit( 0 );

        }
    }
}
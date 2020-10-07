#include "dns.h"

using namespace std;

int port = 53;
string file = "";
string server = "";
shared_timed_mutex mutex;

string getDnsIp( const char* server ){
    struct addrinfo hints, *res;
    memset( &hints, 0, sizeof ( hints ) );
    // STACI ipv4 ?
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags |= AI_CANONNAME;
    
    if( getaddrinfo ( server, NULL, &hints, &res ) != 0 ){
        perror ("getaddrinfo");
        exit( 12 );
    }
    
    void* ptr;
    string addrstr;
    
    inet_ntop( res->ai_family, res->ai_addr->sa_data, (char*) addrstr.c_str(), 100 );

    ptr = &( ( struct sockaddr_in * ) res->ai_addr )->sin_addr;
    inet_ntop (res->ai_family, ptr, (char*) addrstr.c_str(), 100);
    return addrstr.c_str();
}

void getArguments( int argc, char** argv ){
    int arg;
    // TODO test -p -f file treba
    while( ( arg = getopt( argc, argv, "s:p:f:" ) ) != -1 ){
        switch( arg ){
            case 's':
                server = optarg;
                server = getDnsIp( server.c_str() );
                break;
            case 'p':
                port = atoi( optarg );
                break;
            case 'f':
                file = optarg;
                break;
            default:
                // TODO OSETRENI
                cout << arg;
                exit( 10 );
        }
    }

    if( server == "" ){
        cout << "CHYBA";
        exit( 11 );
    } else if ( file == "" ){
        cout << "CHYBA";
        exit( 11 );
    }

}

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

int searchFile( string url ){
    
    vector<string> domain = explode( url, '.' );

    //cout << nationalDomain + "\n";
    //cout << domain + "\n";

    ifstream infile( file );
    string line;

    while( getline( infile, line ) ){
        if( line[ 0 ] == '#' || line.empty() ){
            //skip
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

int main( int argc, char **argv ){
    
    getArguments( argc, argv );

    if( argv[ 1 ] == NULL ){
        cout << "Chybi port argument";
        exit( 5 );
    }

    int socketfd, new_socket, fread;
    struct sockaddr_in my_addr, cliaddr, test;
    int opt = 1;
    int addrlen = sizeof( my_addr );
    char buffer[ 1024 ] = {0};

    // TODO zkontrolovat podminku
    if( ( socketfd = socket( AF_INET, SOCK_DGRAM, 0 ) ) < 0 ){
        cout << "Error with socket creation";
        exit( 10 );
    }
    memset( &my_addr, 0, sizeof( my_addr ) );
    
        
    my_addr.sin_family = AF_INET;
    my_addr.sin_addr.s_addr = INADDR_ANY;
    my_addr.sin_port = htons( port );


    if( bind( socketfd, ( struct sockaddr *) &my_addr, sizeof( my_addr ) ) < 0 ){
        cout << "Bind error";
        exit( errno );
    }
    
    while( 1 ){
        
        memset( &cliaddr, 0, sizeof( cliaddr ) );
        int len = sizeof( cliaddr );

        int clientpacket = recvfrom( socketfd, buffer, 1024, 0, ( struct sockaddr *) &cliaddr, ( socklen_t *) &len );

        cout << "Zprava prijata \n";

        int pid;
        if( ( pid = fork() ) > 0 ){
            cout << "Entering parent \n";
        } else if ( pid == 0 ){
            cout << "Entering child \n";

            dnshdr* pd = ( dnshdr* ) buffer;

            int i = 13;
            int last = i;
            string domain;
            int length = buffer[ i - 1 ];

            while( length != 0 ){
                while( i < last + length ){
                    domain = domain + buffer[ i ];
                    i++;
                }
                length = buffer[ i ];
                i++;
                last = i;
                if( length != 0 ){
                    domain = domain + '.';
                }
            }

            int a = 0;

            if( searchFile( domain ) == 0 ){
                perror( "CHYBA" );
                exit( 20 );
            }

            int newsocket;
            if( ( newsocket = socket( AF_INET, SOCK_DGRAM, 0 ) ) < 0 ){
                cout << "Error with socket creation";
                exit( 10 );
            }

            memset( &test, 0, sizeof( test ) );

            test.sin_family = AF_INET;
            test.sin_addr.s_addr = inet_addr( server.c_str() );
            test.sin_port = htons( 53 );

            // TODO udp je 8 bajtu header
            sendto( newsocket, buffer, clientpacket + 8, 0, ( const struct sockaddr * ) &test, len );
            
            int toclient = recvfrom( newsocket, (char *)buffer, 1024, 0, ( struct sockaddr *) &test, ( socklen_t *) &test );

            sendto( socketfd, buffer, toclient, 0, ( const struct sockaddr * ) &cliaddr, sizeof( cliaddr ) );

            cout << "Child end";
            close( socketfd );
            exit( 0 );

        }
    }

}
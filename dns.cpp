#include <iostream>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>

using namespace std;

int main( int argc, char *argv[] ){
    
    if( argv[ 1 ] == NULL ){
        cout << "Chybi port argument";
        exit( 5 );
    }

    int socketfd, new_socket, fread;
    struct sockaddr_in my_addr, cliaddr;
    int opt = 1;
    int port = atoi( argv[ 1 ] );
    int addrlen = sizeof( my_addr );
    char buffer[ 1024 ] = {0};

    memset( &my_addr, 0, sizeof( my_addr ) );
    memset( &cliaddr, 0, sizeof( cliaddr ) );

    if( ( socketfd = socket( AF_INET, SOCK_DGRAM, 0 ) ) == 0 ){
        cout << "Error with socket creation";
        exit( 10 );
    }

    my_addr.sin_family = AF_INET;
    my_addr.sin_addr.s_addr = INADDR_ANY;
    my_addr.sin_port = htons( port );

    if( bind( socketfd, ( struct sockaddr *)&my_addr, sizeof( my_addr ) ) < 0 ){
        cout << "Bind error";
        exit( errno );
    }

    int len = sizeof( cliaddr );

    while( 1 ){
        recvfrom( socketfd, (char *)buffer, 1024, MSG_WAITALL, ( struct sockaddr *) &cliaddr, ( socklen_t *) &len );

        fread = read( socketfd, buffer, 1024 );

        cout << buffer;

        sendto( socketfd, "Affirmative", 10, MSG_CONFIRM, (const struct sockaddr *) &cliaddr, len); 

    }

}
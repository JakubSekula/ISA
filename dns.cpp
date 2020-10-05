#include <iostream>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>

using namespace std;

typedef struct {
    uint16_t id;
# if __BYTE_ORDER ==  __BIG_ENDIAN
    uint16_t qr:1;
    uint16_t opcode:4;
    uint16_t aa:1;
    uint16_t tc:1;
    uint16_t rd:1;
    uint16_t ra:1;
    uint16_t zero:3;
    uint16_t rcode:4;
# elif __BYTE_ORDER == __LITTLE_ENDIAN
    uint16_t rd:1;
    uint16_t tc:1;
    uint16_t aa:1;
    uint16_t opcode:4;
    uint16_t qr:1;
    uint16_t rcode:4;
    uint16_t zero:3;
    uint16_t ra:1;
# else
#  error "Adjust your <bits/endian.h> defines"
# endif
    uint16_t qcount;    /* question count */
    uint16_t ancount;    /* Answer record count */
    uint16_t nscount;    /* Name Server (Autority Record) Count */ 
    uint16_t adcount;    /* Additional Record Count */
    uint8_t length;
} dnshdr;

// 97 - 104

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

    // TODO zkontrolovat podminku
    if( ( socketfd = socket( AF_INET, SOCK_DGRAM, 0 ) ) < 0 ){
        cout << "Error with socket creation";
        exit( 10 );
    }

    memset( &my_addr, 0, sizeof( my_addr ) );
    memset( &cliaddr, 0, sizeof( cliaddr ) );

    my_addr.sin_family = AF_INET;
    my_addr.sin_addr.s_addr = INADDR_ANY;
    my_addr.sin_port = htons( port );

    if( bind( socketfd, ( struct sockaddr *)&my_addr, sizeof( my_addr ) ) < 0 ){
        cout << "Bind error";
        exit( errno );
    }

    int len = sizeof( cliaddr );

    recvfrom( socketfd, (char *)buffer, 1024, 0, ( struct sockaddr *) &cliaddr, ( socklen_t *) &len );


    dnshdr* pd = ( dnshdr* ) buffer;

    int i = 13;
    int last = i;
    string domain;

    while( pd->length != 0 ){
        while( i < last + pd->length ){
            //printf( "%c", buffer[ i ] );
            domain = domain + buffer[ i ];
            i++;
        }
        pd->length = buffer[ i ];
        last = i + 1;
        if( pd->length != 0 ){
            domain = domain + ".";
        }
    }

    printf( "%s \n", domain.c_str() );

    sendto( socketfd, buffer, 1024, MSG_CONFIRM, (const struct sockaddr *) &cliaddr, len); 

    return 0;

}
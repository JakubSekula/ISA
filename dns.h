#include <iostream>
#include <vector>
#include <fstream>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

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
} dnshdr;
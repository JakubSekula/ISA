/******************************************************************************
 * Project: Filtrující DNS resolver                                           *
 * Subject: ISA - Network Applications and Network Administration             *
 * Rok:     2020/2021                                                         *
 * Authors:                                                                   *
 *			Jakub Sekula  (xsekul01) - xsekul01@stud.fit.vutbr.cz             *
 ******************************************************************************/

/**
 * @file dns.h
 * @author Jakub Sekula( xsekul01 )
 * @date 10.10.2020
 * @brief Packet structure
 */

#include <iostream>
#include <vector>
#include <fstream>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

// DNS packet header
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
# endif
    uint16_t qcount;
    uint16_t ancount;
    uint16_t nscount;
    uint16_t adcount;
} dnshdr;
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
 * @brief Includes
 */

#include <iostream>
#include <vector>
#include <fstream>
#include <netdb.h>
#include <unistd.h>
#include <csignal>
#include <string.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <boost/algorithm/string.hpp>
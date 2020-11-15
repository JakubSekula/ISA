#!/usr/bin/env python3

###############################################################################
 # Project: Filtrující DNS resolver                                           #
 # Subject: ISA - Network Applications and Network Administration             #
 # Rok:     2020/2021                                                         #
 # Authors:                                                                   #
 #			Jakub Sekula  (xsekul01) - xsekul01@stud.fit.vutbr.cz             #
 ###############################################################################

##
 # @file test.py
 # @author Jakub Sekula( xsekul01 )
 # @date 10.10.2020
 # @brief testovací skript
 ##

import os
import re
import sys
import time
import subprocess
import signal
import getopt
import random

# počet neúspěšných testů
failed = 0
# počet úspěšných testů
succ = 0

##
 # Funkce prohledá soubor blacklist domén a jestliže je nalezena doména, která má být odfiltrovaná, tak se vrací FAIL, jinak OK
 # @param domain doména
 # @return "OK" při nenalezení domény
 # @return "FAIL" při nalezení domény v souboru blacklist
##
def searchFile( domain ):
    global blacklisted

    domain = domain.split( '.' )

    for parsedLine in blacklisted:
        if( parsedLine in [ '\n', '\r\n' ] ):
            continue
        else:
            parsedLine = parsedLine.split( '.' )
            if( parsedLine[ 0 ] == '#' ):
                continue
        if( len( domain ) < len( parsedLine ) ):
            continue
        else:
            i = len( domain ) - 1
            p = len( parsedLine ) - 1
            while( p >= 0 ):
                if( domain[ i ] == parsedLine[ p ] ):
                    i -= 1
                    p -= 1
                    if( p < 0 ):
                        return "FAIL"
                else:
                    break
    return "OK"

##
 # Funkce prohledá výstup subprocesu, z kterého pouštím dotazy na server, jestliže v out authoritative tak byla navrácena alespoň 1 ip
 # @param out výstup
 # @param domain doména
 # @return "OK" při úspěchu
 # @return "FAIL" při neúspěchu
##
def searchOut( out, domain ):
    global failed
    global succ
    out = out.decode( "utf8" )
    
    expected = searchFile( domain )
    
    if( re.search( 'authoritative', out ) ):
        if( expected == "OK" ):
            succ += 1
        else:
            failed += 1
        return "OK " + expected
    else:
        if( expected == "FAIL" ):
            succ += 1
        else:
            failed += 1
        return "FAIL " + expected
    return

##
 # odstranění znaku nového řádku na konci stringu
 # @param file path ke složce
 # @return seznam domén
##
def lineList( file ):
    domains = []
    
    for line in fileThis:
        if( line[ -1 ] == '\n' ):
            line = line[ :-1 ]
            domains.append( line )
        else:
            domains.append( line )
    return domains

if( len( sys.argv ) != 5 ):
    print( "Run scripts as: test.py [port] [domains file] [blacklisted domains] [DNS resolver]" )
    exit( 0 )

## získávání parametrů
port = sys.argv[ 1 ]
DomainsFile = sys.argv[ 2 ]
BlacklistDomains = sys.argv[ 3 ]

fileThis = open( DomainsFile, 'r' )
DOMAINS = lineList( fileThis )

fileThis = open( BlacklistDomains, 'r' )
blacklisted = lineList( fileThis )

DNS = sys.argv[ 4 ]

command = "./dns -s " + DNS +" -f tests/blacklist -p " + port

## jestliže není binárka dns ve složce, tak spustím příkaz make
if( not os.path.isfile( "dns" ) ):
    print( "Soubor dns nenalezen" )
    exit( 0 )
else:
    proc = subprocess.Popen( [ "./dns", "-s", DNS, "-f", "tests/blacklist", "-p", port ] )

print( "Running tests: " )
for domain in DOMAINS:
    pro = subprocess.Popen( [ "nslookup", "-port=" + str( port ), "-type=a", domain, "localhost" ], stdout=subprocess.PIPE )
    ## výstup ze subprocessu
    out, err = pro.communicate()
    result = searchOut( out, domain )
    print( "   " + domain, result )
    ## ukončím process
    pro.send_signal( signal.SIGTERM )   

print( "-------------------------" )
        
print( "Succesful: " + str( succ ) )
print( "Failed: " + str( failed ) )

## ukončím process
proc.send_signal( signal.SIGTERM )  


exit( 0 )
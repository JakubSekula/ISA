#!/usr/bin/env python3

import os
import re
import subprocess
import signal
import random

BLACKLIST = dict()
DOMAINS = dict()
RUNS = ""

def searchFile( domain ):
    
    for parsedLine in BLACKLIST[ 1 ]:
        parsedLine = parsedLine.split( '.' )

        if( parsedLine[ 0 ] == '#' or parsedLine in [ '\n', '\r\n' ] ):
            ...
        else:
            domain = domain.split( '.' )
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

def searchOut( out, domain ):
    out = out.decode( "utf8" )

    expected = searchFile( domain )

    if( re.search( 'authoritative', out ) ):
        return "OK " + expected
    else:
        return "FAIL " + expected
    return

def lineList( file ):
    domains = []
    
    for line in fileThis:
        if( line[ -1 ] == '\n' ):
            line = line[ :-1 ]
            domains.append( line )
        else:
            domains.append( line )
    return domains

def executeThem():
    global RUNS
    file1 = open( 'tests/' + RUNS, 'r' )
    Lines = file1.readlines()

    for line in Lines:
        os.system( line )
    os.system( "" )

listdir = os.listdir( "tests/" )

for file in listdir:
    if( re.search( "^executio", file ) ):
        fileThis = open( "tests/" + file, 'r' )
        RUNS = lineList( fileThis )
    else:
        number = file[ -1 ] 
        if( re.search( "^blackl", file ) ):
            fileThis = open( "tests/" + file, 'r' )
            BLACKLIST[ 1 ] = lineList( fileThis )
        elif( re.search( "^doma", file ) ):
            fileThis = open( "tests/" + file, 'r' )
            DOMAINS[ int( number ) ] = lineList( fileThis )
        else:
            ...

port = random.randint( 1024, 65535 )
if( not os.path.isfile( "dns" ) ):
    os.system( "make" )
else:
    proc = subprocess.Popen( [ "./dns -s 1.1.1.1 -p " + str( port ) + " -f tests/blacklist" ], shell=True )

for domains in DOMAINS:
    print( "test" + str( domains ) + ": " )
    for domain in DOMAINS[ domains ]:
        pro = subprocess.Popen( [ "nslookup -port=" + str( port ) + " -type=a " + domain + " localhost" ], stdout=subprocess.PIPE, shell=True )
        out, err = pro.communicate()
        result = searchOut( out, domain )
        print( "   " + domain, result )
        pro.terminate()

os.killpg( os.getpgid( proc.pid ), signal.SIGTERM )

exit( 0 )
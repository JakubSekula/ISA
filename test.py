#!/usr/bin/env python3

import os
import re
import subprocess

BLACKLIST = dict()
DOMAINS = dict()
RUNS = ""

def executeThem():
    global RUNS
    file1 = open( 'tests/' + RUNS, 'r' )
    Lines = file1.readlines()

    for line in Lines:
        os.system( line )
    os.system( "" )

for file in os.listdir( "tests/" ):
    if( file.startswith( "executio" ) ):
        RUNS = file
    else:
        number = file[ -1 ]
        file = file[ :-1 ]
        if( file.startswith( "blackli" ) ):
            BLACKLIST[ number ] = file
        elif( file.startswith( "domai" ) ):
            DOMAINS[ number ] = file
        else:
            ...

#executeThem()

p = subprocess.Popen(["nslookup -port=8080 -type=a google.com localhost"], stdout=subprocess.PIPE, shell=True)
port = 1025
if( not os.path.isfile( "dns" ) ):
    os.system( "make" )
else:
    while( port < 65535 ):
        try: 
            proc = subprocess.Popen( ["./dns -s 1.1.1.1 -p 8081 -f domains" ], shell=True)
            break
        except:
            port = port + 1

out, err = p.communicate()

proc.terminate()

print( out )
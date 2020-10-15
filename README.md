# ISA

### Program DNS
Program DNS is a server thats receives dns packet with dns question, then id check if domain is blacklisted and if it is not it returns ip address for a sever.

### How to run DNS server
./dns -s server [-p port] -f filter_file<br/>

for example: 
<br/>./dns -s 1.1.1.1 -p 8080 -f domains
<br/>./dns -s 8.8.4.4 -p 2000 -f file.txt
<br/>./dns -s server_name -f domains


### Files included
###
* dns.cpp
* dns.h
* makefile
* README.md
* manual.pdf
* test.py

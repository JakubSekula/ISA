# ISA

### Program DNS
Program DNS is a server that receives dns packet with dns query, then it checks if domain is blacklisted and if it is not it returns ip address for a sever.

### How to run DNS server
./dns -s server [-p port] -f filter_file<br/>

for example: 
<br/>./dns -s 1.1.1.1 -p 8080 -f tests/blacklist
<br/>./dns -s 8.8.4.4 -p 2000 -f tests/blacklist
<br/>./dns -s dns.google -f tests/blacklist


### Files included
###
* dns.cpp
* dns.hpp
* ext/dnshrd.hpp
* ext/gpl-3.0.txt
* Makefile
* README.md
* manual.pdf
* test.py
* Doxyfile
* tests/domains1
* tests/blacklist

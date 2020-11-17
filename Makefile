proj=dns

make:
	@g++ -Wall -Wextra -std=c++17 $(proj).cpp -o $(proj)
docu:
	@doxygen
clean:
	@rm $(proj)
test:
	@make
	@python3 test.py 61124 tests/domains1 tests/blacklist 8.8.4.4
	@python3 test.py 61124 tests/domains1 tests/blacklist 2001:4860:4860::8888
pack: 
	tar -cf xsekul01.tar Doxyfile dns.cpp dns.hpp ext/dnshdr.hpp ext/gpl-3.0.txt Makefile manual.pdf README.md test.py tests/domains1 tests/blacklist
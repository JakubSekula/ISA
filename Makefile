proj=dns

make:
	@g++ -Wall -Wextra -std=c++17 $(proj).cpp -o $(proj)
docu:
	@doxygen
clean:
	@rm $(proj)
test:
	python3 test.py 8080 tests/domains1 tests/blacklist 8.8.4.4
pack:
	tar -cf xsekul01.tar dns.cpp dns.hpp ext/dnshdr.hpp Doxyfile ext/gpl-3.0.txt Makefile manual.pdf README.md test.py tests/domains1 tests/blacklist
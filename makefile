proj=dns

make:
	@g++ -Wall -Wextra -Werror -std=c++17 $(proj).cpp -o $(proj)
run:
	@g++ $(proj).cpp -o $(proj)
	./dns -s 1.1.1.1 -p 8080 -f domains
done:
	tar -zcvf xsekul01.tar dns.cpp dns.h makefile README.md
clean:
	@rm $(proj)
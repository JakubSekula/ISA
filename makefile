proj=dns

make:
	@g++ -Wall -Wextra -Werror -std=c++17 $(proj).cpp -o $(proj)
docu:
	@doxygen
clean:
	@rm $(proj)
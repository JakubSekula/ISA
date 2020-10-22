proj=dns

make:
	@g++ -Wall -Wextra -std=c++17 $(proj).cpp -o $(proj)
docu:
	@doxygen
clean:
	@rm $(proj)
test:
	python3 test.py
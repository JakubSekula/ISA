proj=dns

make:
	@g++ $(proj).cpp -o $(proj)
clean:
	@rm $(proj)
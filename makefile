proj=dns

make:
	@g++ $(proj).cpp -o $(proj)
run:
	@g++ $(proj).cpp -o $(proj)
	./dns 8080
clean:
	@rm $(proj)
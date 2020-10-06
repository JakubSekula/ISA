proj=dns

make:
	@g++ $(proj).cpp -o $(proj)
run:
	@g++ $(proj).cpp -o $(proj)
	./dns -s 8.8.8.8 -p 8080 -f domains
clean:
	@rm $(proj)
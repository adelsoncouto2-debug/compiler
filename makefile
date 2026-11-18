all:
	g++ -std=c++17 -Wall -Wextra -I includes -c src/lex.cpp -o exe/lex.o
	g++ -std=c++17 -Wall -Wextra -I includes -c src/main.cpp -o exe/main.o
	g++ exe/lex.o exe/main.o -o exe/compiler
	rm -f exe/*.o

.PHONY: all clean

clean:
	rm -f exe/*.o exe/compiler exe/main

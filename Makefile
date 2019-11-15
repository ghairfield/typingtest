.PHONY: run clean

CC=gcc
FLAGS=-Wall -Wextra -g

spd : screen.o main.o
	$(CC) $(FLAGS) $^ -o $@

main.o : src/main.c src/screen.h
	$(CC) $(FLAGS) -c $< -o $@

screen.o : src/screen.c src/screen.h
	$(CC) $(FLAGS) -c $< -o $@

clean:
	rm spd screen.o main.o

run : 
	valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes --verbose ./spd

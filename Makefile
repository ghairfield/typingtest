CC=gcc
CFLAGS=-Wall -Wextra -g -std=c99
vpath %.c src
vpath %.h src
vpath %.py data

spd : main.o game.o screen.o words.o
	$(CC) $(CFLAGS) -o $@ $^ -lbsd

main.o : main.c game.h
	$(CC) $(CFLAGS) -c -o $@ $<

game.o : game.c game.h screen.h
	$(CC) $(CFLAGS) -c -o $@ $<

screen.o : screen.c screen.h
	$(CC) $(CFLAGS) -c -o $@ $<

words.o : words.c words.h
	$(CC) $(CFLAGS) -c -o $@ $<

.PHONY: tags
tags:
	ctags -R

.PHONY: clean 
clean:
	rm spd main.o game.o screen.o words.o

.PHONY: run
run : 
	valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes --verbose ./spd

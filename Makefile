CC=gcc
CFLAGS=-Wall -Wextra -g
vpath %.c src
vpath %.h src
vpath %.py data

spd : main.o game.o screen.o
	$(CC) $(CFLAGS) -o $@ $^

main.o : main.c game.h
	$(CC) $(CFLAGS) -c -o $@ $<

game.o : game.c game.h screen.h
	$(CC) $(CFLAGS) -c -o $@ $<

screen.o : screen.c screen.h
	$(CC) $(CFLAGS) -c -o $@ $<

.PHONY: tags
tags:
	ctags -R

.PHONY: clean 
clean:
	rm spd main.o game.o screen.o

.PHONY: run
run : 
	valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes --verbose ./spd

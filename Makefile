<<<<<<< HEAD
.PHONY: run clean

CC=gcc
CFLAGS=-Wall -Wextra -g
vpath %.c src
vpath %.h src

spd : main.o screen.o
	$(CC) $(CFLAGS) -o $@ $^

main.o : main.c screen.h
	$(CC) $(CFLAGS) -c -o $@ $<

screen.o : screen.c screen.h
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm spd main.o screen.o	

run : 
	valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes --verbose ./spd
=======
CC=gcc -Wall -Wextra -g -std=c99

spd : src/speedtest.c src/fileio.c
	$(CC) src/speedtest.c src/fileio.c -o spd

test:
	valgrind --leak-check=yes --show-reachable=yes --num-callers=20 --track-fds=yes ./spd
>>>>>>> origin/master

.PHONY: run clean

CC=cc -Wall -Wextra -g

spd : 
	$(CC) src/speedtest.c -o $@

clean:
	rm spd

run : 
	valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes --verbose ./spd

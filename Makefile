CC=gcc -Wall -Wextra -g -std=c99

spd : src/speedtest.c src/fileio.c
	$(CC) src/speedtest.c src/fileio.c -o spd

test:
	valgrind --leak-check=yes --show-reachable=yes --num-callers=20 --track-fds=yes ./spd

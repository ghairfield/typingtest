CC=cc -Wall -Wextra -g

#spd : src/speedtest.c
#	$(CC) src/speedtest.c -o spdtst

practice : src/speedtest.c
	$(CC) src/speedtest.c -o spd

run : 
	valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes --verbose ./spd

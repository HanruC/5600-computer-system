CC = gcc
CFLAGS = -Wall -Wextra

all: client server

client: c.c
	$(CC) c.c -o c -pthread

server: s.c
	$(CC) s.c -o s -pthread

clean:
	rm -f c s
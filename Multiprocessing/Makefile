CC=gcc
CFLAGS=-Wall

all: multiprocessing polybius

multiprocessing: multiprocessing.c queue.c
	$(CC) $(CFLAGS) multiprocessing.c queue.c -o multiprocessing

polybius: polybius.c
	$(CC) $(CFLAGS) polybius.c -o polybius

clean:
	rm -f multiprocessing polybius

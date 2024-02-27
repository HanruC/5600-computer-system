CC=gcc
CFLAGS=-Wall -pthread

all: program

program: pqueuepmain.c 
	$(CC) $(CFLAGS) pqueuepmain.c  -o pqmain 

clean:
	rm -f pqmain 
	touch pqmain
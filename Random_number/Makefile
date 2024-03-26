CC = clang 
CFLAGS = -Wall
TARGET = genRand

all: $(TARGET) 

$(TARGET): genRand-Chen.c
	$(CC) $(CFLAGS) -o $(TARGET) genRand-Chen.c

clean:
	rm -f $(TARGET) 
	touch genRand-Chen.c
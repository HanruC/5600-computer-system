CC = clang
CFLAGS = -Wall
TARGET = qtest

all: $(TARGET)

$(TARGET): queuetest.c queue.c
	$(CC) $(CFLAGS) queuetest.c queue.c -o $(TARGET)

clean:
	rm -f $(TARGET) *.o
	touch $(TARGET)

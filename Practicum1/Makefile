CC=gcc
CFLAGS=-Wall
TARGET=message_test
SOURCES=main.c message.c store.c retrieve.c cache.c

all: $(TARGET)

$(TARGET): $(SOURCES)
	$(CC) $(CFLAGS) -o $(TARGET) $(SOURCES)

clean: 
	rm -f $(TARGET)
	touch $(SOURCES)

CC = clang
CFLAGS = -Wall
TARGET = cipher

SOURCES = cipher.c polybius.c

all: $(TARGET)

$(TARGET):
	$(CC) $(CFLAGS) -o $(TARGET) $(SOURCES)

clean:
	rm -f $(TARGET)
	touch $(TARGET)

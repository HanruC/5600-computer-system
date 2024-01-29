CC=clang
CFLAGS=-Wall 
TARGET1 = alloc-mem-Chen
TARGET2 = load-mem-Chen

all: $(TARGET1) $(TARGET2)

$(TARGET1): $(TARGET1).c
	$(CC) $(CFLAGS) -o $(TARGET1) $(TARGET1).c 

$(TARGET2): $(TARGET2).c
	$(CC) $(CFLAGS) -o $(TARGET2) $(TARGET2).c

clean: 
	rm -f $(TARGET1) $(TARGET2) 
	touch $(TARGET1).c $(TARGET2).c
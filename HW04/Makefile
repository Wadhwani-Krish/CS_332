
CC = gcc
CFLAGS = -Wall -pthread
TARGET = hw4
SOURCE = hw4.c

all: $(TARGET)

$(TARGET): $(SOURCE)
	$(CC) $(CFLAGS) $(SOURCE) -o $(TARGET)

run: $(TARGET)
	./$(TARGET)

clean:
	rm -f $(TARGET) average.txt

.PHONY: all run clean

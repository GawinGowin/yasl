CC = gcc
CFLAGS = -Wall -Wextra -O2 -std=c11
TARGET = rgb_size_detect

all: $(TARGET)

$(TARGET): rgb_size_detect.c
	$(CC) $(CFLAGS) -o $(TARGET) rgb_size_detect.c -lm

clean:
	rm -f $(TARGET)

.PHONY: all clean

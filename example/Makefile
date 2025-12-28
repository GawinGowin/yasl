CC = gcc
CFLAGS = -Wall -Wextra -O2 -std=c11
TARGET = display_b64.out

all: $(TARGET)

$(TARGET): display_b64.c
	$(CC) $(CFLAGS) -o $(TARGET) display_b64.c -lm

clean:
	rm -f $(TARGET)

.PHONY: all clean

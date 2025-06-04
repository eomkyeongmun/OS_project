CC = gcc
CFLAGS = -Iinclude -Wall -Wextra
LDFLAGS = -lpthread
SRC = $(wildcard src/*.c) src/stack.c

OBJ = $(SRC:.c=.o)
TARGET = mini_os

$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

clean:
	rm -f $(OBJ) $(TARGET)

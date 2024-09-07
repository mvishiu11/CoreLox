CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -I./src/include

SRC = $(wildcard src/*.c)
OBJ = $(SRC:.c=.o)
TARGET = carbonlox

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^

clean:
	rm -f $(OBJ) $(TARGET)

test: $(TARGET)
	./tests/test_runner.sh

lint:
	cppcheck --enable=all --inconclusive --std=c99 src/* tests/*

format:
	find . -name "*.c" -o -name "*.h" | xargs clang-format -i

.PHONY: all clean test
CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -I./src/include

SRC = $(wildcard src/*.c)
OBJ = $(filter-out src/main.o, $(SRC:.c=.o))
TARGET = corelox

# Benchmarking variables
BENCHMARKS_SRC = $(wildcard benchmarks/*.c)
BENCHMARKS_OBJ = $(BENCHMARKS_SRC:.c=.o)
BENCHMARK_TARGET = benchmarks.out

all: $(TARGET)

$(TARGET): $(OBJ) src/main.o
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJ) src/main.o

# Rule to compile benchmarks without linking src/main.o
bench: $(BENCHMARKS_OBJ) $(OBJ)
	$(CC) $(CFLAGS) -o $(BENCHMARK_TARGET) $(BENCHMARKS_OBJ) $(OBJ)

# Compile the .o files for the benchmark folder
benchmarks/%.o: benchmarks/%.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ) $(TARGET)
	rm -f $(BENCHMARKS_OBJ) $(BENCHMARK_TARGET)

test: $(TARGET)
	./scripts/test_runner.sh

lint:
	cppcheck --enable=all --inconclusive --std=c99 src/* tests/*

format:
	find . -name "*.c" -o -name "*.h" | xargs clang-format -i

.PHONY: all clean test lint format bench
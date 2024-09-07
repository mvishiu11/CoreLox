#!/bin/bash

echo "Running tests..."
gcc -o test_parser tests/test_parser.c src/parser.c -I./src/include
./test_parser

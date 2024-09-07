#ifndef corelox_line_info_h
#define corelox_line_info_h

#include "common.h"
#include <stdlib.h>
#include <stdio.h>

typedef double Value;

typedef struct {
  int line;
  int run_length;
} LineInfo;

typedef struct {
  int count;
  int capacity;
  LineInfo* lines;
} LineInfoArray;

void initLineInfoArray(LineInfoArray* array);

void freeLineInfoArray(LineInfoArray* array);

void printLineInfo(LineInfo value);

#endif
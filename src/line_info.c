#include "line_info.h"

void initLineInfoArray(LineInfoArray* array) {
  array->lines = NULL;
  array->capacity = 0;
  array->count = 0;
}

void freeLineInfoArray(LineInfoArray* array) {
  if (array->lines != NULL) {
    free(array->lines);
    array->lines = NULL;
  }
  array->capacity = 0;
  array->count = 0;
}

void printLineInfo(LineInfo value) {
  printf("Line: %d, Run Length: %d\n", value.line, value.run_length);
}
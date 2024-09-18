#include "program.h"

#include <stdio.h>

int main() {
  double num = 3.14;
  Value value = NUMBER_VAL(num);
  printf("Value: %f\n", AS_NUMBER(value));
}
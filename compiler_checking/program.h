#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

/**
 * @file This file is meant for inspecting Assembly code generated by the C compiler.
 *
 * The purpose of this file is to provide a simple way to inspect the Assembly code
 * of particular C code snippets. This is needed for some specific optimizations used
 * in the CoreLox project, such as the NaN-boxing optimization.
 */

typedef uint64_t Value;

#define AS_NUMBER(value) valueToNum(value)

#define NUMBER_VAL(num) numToValue(num)

static inline Value numToValue(double num) {
  Value value;
  memcpy(&value, &num, sizeof(double));
  return value;
}

static inline double valueToNum(Value value) {
  double num;
  memcpy(&num, &value, sizeof(Value));
  return num;
}
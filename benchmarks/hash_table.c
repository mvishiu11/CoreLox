#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include "table.h"
#include "memory.h"

double run_benchmark() {
  clock_t start = clock();

  Table table;
  initTable(&table);

  ObjString* key = copyString("key", 3);
  Value value = BOOL_VAL(true);

  tableSet(&table, key, value);

  Value result;
  tableGet(&table, key, &result);

  freeTable(&table);

  freeObject(&key->obj);

  clock_t end = clock();
  double time_spent = (double)(end - start) / CLOCKS_PER_SEC;
  return time_spent;
}

int main() {
  double time = run_benchmark();
  printf("Total time: %f\n", time);
  return 0;
}
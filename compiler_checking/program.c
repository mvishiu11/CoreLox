#include "program.h"
#include <stdio.h>

int main() {
    double num = 3.14;
    Value value = numToValue(num);
    printf("Value: %f\n", valueToNum(value));
}
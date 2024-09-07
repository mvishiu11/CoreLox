#include "vm.h"

#include <stdio.h>

#include "common.h"
#include "debug.h"

VM vm;

void initVM() {}

void freeVM() {}

static InterpretResult run() {
#define READ_BYTE() (*vm.ip++)
#define READ_CONSTANT() (vm.chunk->constants.values[READ_BYTE()])
#define READ_CONSTANT_LONG()                           \
  ({                                                   \
    uint8_t byte1 = READ_BYTE();                       \
    uint8_t byte2 = READ_BYTE();                       \
    uint8_t byte3 = READ_BYTE();                       \
    vm.chunk->constants.values[(byte1 << 16) | (byte2 << 8) | byte3]; \
  })

  for (;;) {
#ifdef DEBUG_TRACE_EXECUTION
    printf("          ");
    for (Value* slot = vm.chunk->constants.values;
         slot < vm.chunk->constants.values + vm.chunk->constants.count; slot++) {
      printf("[ ");
      printValue(*slot);
      printf(" ]");
    }
    printf("\n");
    disassembleInstruction(vm.chunk, (int)(vm.ip - vm.chunk->code));
#endif

    uint8_t instruction;
    switch (instruction = READ_BYTE()) {
      case OP_CONSTANT_LONG: {
        Value constant = READ_CONSTANT_LONG();
        printValue(constant);
        printf("\n");
        break;
      }
      case OP_CONSTANT: {
        Value constant = READ_CONSTANT();
        printValue(constant);
        printf("\n");
        break;
      }
      case OP_RETURN: {
        return INTERPRET_OK;
      }
    }
  }

#undef READ_BYTE
#undef READ_CONSTANT
#undef READ_CONSTANT_LONG
}

InterpretResult interpret(Chunk* chunk) {
  vm.chunk = chunk;
  vm.ip = vm.chunk->code;
  return run();
}
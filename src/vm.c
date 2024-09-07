#include "vm.h"

#include <stdio.h>

#include "common.h"
#include "compiler.h"
#include "debug.h"
#include "memory.h"

VM vm;

void resetStack() { vm.stackTop = vm.stack; }

void initVM() {
  vm.stackCapacity = STACK_MAX;
  vm.stack = GROW_ARRAY(Value, NULL, 0, vm.stackCapacity);
  resetStack();
}

void freeVM() { FREE_ARRAY(Value, vm.stack, vm.stackCapacity); }

void push(Value value) {
  if (vm.stackTop - vm.stack >= vm.stackCapacity) {
    int oldCapacity = vm.stackCapacity;
    vm.stackCapacity = GROW_CAPACITY(oldCapacity);
    vm.stack = GROW_ARRAY(Value, vm.stack, oldCapacity, vm.stackCapacity);
    if (vm.stack == NULL) {
      fprintf(stderr, "Failed to reallocate memory for the stack.\n");
      exit(1);
    }
    vm.stackTop = vm.stack + oldCapacity;
  }
  *vm.stackTop = value;
  vm.stackTop++;
}

Value pop() {
  vm.stackTop--;
  return *vm.stackTop;
}

static InterpretResult run() {
#define READ_BYTE() (*vm.ip++)
#define READ_CONSTANT() (vm.chunk->constants.values[READ_BYTE()])
#define READ_CONSTANT_LONG()                                          \
  ({                                                                  \
    uint8_t byte1 = READ_BYTE();                                      \
    uint8_t byte2 = READ_BYTE();                                      \
    uint8_t byte3 = READ_BYTE();                                      \
    vm.chunk->constants.values[(byte1 << 16) | (byte2 << 8) | byte3]; \
  })
#define BINARY_OP(op)                                     \
  do {                                                    \
    vm.stackTop[-2] = vm.stackTop[-2] op vm.stackTop[-1]; \
    vm.stackTop--;                                        \
  } while (false)

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
        push(constant);
        break;
      }
      case OP_CONSTANT: {
        Value constant = READ_CONSTANT();
        push(constant);
        break;
      }
      case OP_ADD:
        BINARY_OP(+);
        break;
      case OP_SUBTRACT:
        BINARY_OP(-);
        break;
      case OP_MULTIPLY:
        BINARY_OP(*);
        break;
      case OP_DIVIDE:
        BINARY_OP(/);
        break;
      case OP_NEGATE:
        vm.stackTop[-1] = -vm.stackTop[-1];
        break;
      case OP_RETURN: {
        printValue(pop());
        printf("\n");
        return INTERPRET_OK;
      }
    }
  }

#undef READ_BYTE
#undef READ_CONSTANT
#undef READ_CONSTANT_LONG
#undef BINARY_OPs
}

InterpretResult interpret(const char* source) {
  Chunk chunk;
  initChunk(&chunk);

  if (!compile(source, &chunk)) {
    freeChunk(&chunk);
    return INTERPRET_COMPILE_ERROR;
  }

  vm.chunk = &chunk;
  vm.ip = vm.chunk->code;

  InterpretResult result = run();

  freeChunk(&chunk);
  return result;
}
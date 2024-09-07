#ifndef carbonlox_compiler_h
#define carbonlox_compiler_h

#include "vm.h"
#include "scanner.h"

typedef struct {
  Token current;
  Token previous;
  bool hadError;
  bool panicMode;
} Parser;

bool compile(const char* source, Chunk* chunk);

#endif
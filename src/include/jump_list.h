#ifndef corelox_compiler_h
#define corelox_compiler_h

#include "chunk.h"

typedef struct {
  int count;
  int capacity;
  int* jumps;
} JumpList;

void initJumpList(JumpList* list);
void freeJumpList(JumpList* list);
void addJump(JumpList* list, int offset);
void patchJumps(Chunk* chunk, JumpList* list, int target);

#endif
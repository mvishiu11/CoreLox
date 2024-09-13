#ifndef corelox_jump_h
#define corelox_jump_h

#include "chunk.h"

typedef struct {
  int offset;
  int depth;
} Jump;

typedef struct {
  int count;
  int capacity;
  Jump* jumps;
} JumpList;

void initJumpList(JumpList* list);
void freeJumpList(JumpList* list);
void addJump(JumpList* list, int depth, int offset);

#endif
#include "jump_list.h"

#include "memory.h"

void initJumpList(JumpList* list) {
  list->count = 0;
  list->capacity = 0;
  list->jumps = NULL;
}

void freeJumpList(JumpList* list) {
  FREE_ARRAY(int, list->jumps, list->capacity);
  initJumpList(list);
}

void addJump(JumpList* list, int offset) {
  if (list->capacity < list->count + 1) {
    int oldCapacity = list->capacity;
    list->capacity = GROW_CAPACITY(oldCapacity);
    list->jumps = GROW_ARRAY(int, list->jumps, oldCapacity, list->capacity);
  }

  list->jumps[list->count] = offset;
  list->count++;
}

void patchJumps(Chunk* chunk, JumpList* list, int target) {
  for (int i = 0; i < list->count; i++) {
    int offset = list->jumps[i];
    int jump = target - offset;

    if (jump > UINT16_MAX) {
      error("Too much code to jump over.");
    }

    chunk->code[offset] = (jump >> 8) & 0xff;
    chunk->code[offset + 1] = jump & 0xff;
  }
}
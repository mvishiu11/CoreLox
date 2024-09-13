#include "jump_list.h"

#include "memory.h"

void initJumpList(JumpList* list) {
  list->count = 0;
  list->capacity = 0;
  list->jumps = NULL;
}

void freeJumpList(JumpList* list) {
  FREE_ARRAY(Jump, list->jumps, list->capacity);
  initJumpList(list);
}

void addJump(JumpList* list, int depth, int offset) {
  if (list->capacity < list->count + 1) {
    int oldCapacity = list->capacity;
    list->capacity = GROW_CAPACITY(oldCapacity);
    list->jumps = GROW_ARRAY(Jump, list->jumps, oldCapacity, list->capacity);
  }

  Jump jump = {offset, depth};
  list->jumps[list->count] = jump;
  list->count++;
}
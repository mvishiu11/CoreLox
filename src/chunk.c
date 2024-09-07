#include "chunk.h"

#include <stdlib.h>

#include "memory.h"

void initChunk(Chunk* chunk) {
  chunk->count = 0;
  chunk->capacity = 0;
  chunk->code = NULL;
  initLineInfoArray(&chunk->lines);
  initValueArray(&chunk->constants);
}

void freeChunk(Chunk* chunk) {
  FREE_ARRAY(uint8_t, chunk->code, chunk->capacity);
  freeLineInfoArray(&chunk->lines);
  freeValueArray(&chunk->constants);
  initChunk(chunk);
}

void writeChunk(Chunk* chunk, uint8_t byte, int line) {
  // Handle the bytecode
  if (chunk->capacity < chunk->count + 1) {
    int oldCapacity = chunk->capacity;
    chunk->capacity = GROW_CAPACITY(oldCapacity);
    chunk->code = GROW_ARRAY(uint8_t, chunk->code, oldCapacity, chunk->capacity);
  }

  chunk->code[chunk->count] = byte;
  chunk->count++;

  // Handle the LineInfoArray for RLE encoding
  if (chunk->lines.count == 0 || chunk->lines.lines[chunk->lines.count - 1].line != line) {
    if (chunk->lines.capacity < chunk->lines.count + 1) {
      int oldCapacity = chunk->lines.capacity;
      chunk->lines.capacity = GROW_CAPACITY(oldCapacity);
      chunk->lines.lines =
          GROW_ARRAY(LineInfo, chunk->lines.lines, oldCapacity, chunk->lines.capacity);
    }

    chunk->lines.lines[chunk->lines.count].line = line;
    chunk->lines.lines[chunk->lines.count].run_length = 1;
    chunk->lines.count++;
  } else {
    chunk->lines.lines[chunk->lines.count - 1].run_length++;
  }
}

void writeConstant(Chunk* chunk, Value value, int line) {
  int index = addConstant(chunk, value);

  if (index < 256) {
    writeChunk(chunk, OP_CONSTANT, line);
    writeChunk(chunk, index, line);
  } else {
    writeChunk(chunk, OP_CONSTANT_LONG, line);

    // Write the index as three separate bytes (big-endian)
    writeChunk(chunk, (index >> 16) & 0xFF, line);  // High byte
    writeChunk(chunk, (index >> 8) & 0xFF, line);   // Middle byte
    writeChunk(chunk, index & 0xFF, line);          // Low byte
  }
}

int addConstant(Chunk* chunk, Value value) {
  writeValueArray(&chunk->constants, value);
  return chunk->constants.count - 1;
}

int getLine(Chunk* chunk, int instructionIndex) {
  int offset = 0;

  for (int i = 0; i < chunk->lines.count; i++) {
    offset += chunk->lines.lines[i].run_length;
    if (instructionIndex < offset) {
      return chunk->lines.lines[i].line;
    }
  }

  return -1;
}
#include "object.h"

#include <stdio.h>
#include <string.h>

#include "memory.h"
#include "value.h"
#include "vm.h"
#include "table.h"

#define ALLOCATE_OBJ(type, objectType) (type*)allocateObject(sizeof(type), objectType)
#define ALLOCATE_OBJ_CST_SIZE(type, objectType, size) (type*)allocateObject(size, objectType)

static Obj* allocateObject(size_t size, ObjType type) {
  Obj* object = (Obj*)reallocate(NULL, 0, size);
  object->type = type;

  object->next = vm.objects;
  vm.objects = object;
  return object;
}

static ObjString* allocateString(char* chars, int length, uint32_t hash) {
  ObjString* string = ALLOCATE_OBJ_CST_SIZE(ObjString, OBJ_STRING, sizeof(ObjString) + length + 1);
  string->length = length;
  string->hash = hash;
  memcpy(string->chars, chars, length);
  string->chars[length] = '\0';
  tableSet(&vm.strings, string, NIL_VAL);
  return string;
}

/**
 * @brief Hashes a string using the FNV-1a algorithm.
 *
 * This function computes the hash value of a string using the FNV-1a algorithm.
 * It iterates over each character in the string, XORing the hash with the character
 * and multiplying by a prime number to create a unique hash value.
 *
 * @param key The character array to hash.
 * @param length The length of the character array.
 * @return The hash value of the string.
 */
static uint32_t hashString(const char* key, int length) {
  uint32_t hash = 2166136261u;
  for (int i = 0; i < length; i++) {
    hash ^= (uint8_t)key[i];
    hash *= 16777619;
  }
  return hash;
}

ObjString* copyString(const char* chars, int length) {
  uint32_t hash = hashString(chars, length);
  ObjString* interned = tableFindString(&vm.strings, chars, length, hash);
  if (interned != NULL) return interned;
  char* heapChars = ALLOCATE(char, length + 1);
  memcpy(heapChars, chars, length);
  heapChars[length] = '\0';
  return allocateString(heapChars, length, hash);
}

ObjString* takeString(char* chars, int length) {
  uint32_t hash = hashString(chars, length);
  ObjString* interned = tableFindString(&vm.strings, chars, length,
                                        hash);
  if (interned != NULL) {
    FREE_ARRAY(char, chars, length + 1);
    return interned;
  }
  return allocateString(chars, length, hash);
}

void printObject(Value value) {
  switch (OBJ_TYPE(value)) {
    case OBJ_STRING:
      printf("%s", AS_CSTRING(value));
      break;
  }
}
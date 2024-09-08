#include "object.h"

#include <stdio.h>
#include <string.h>

#include "memory.h"
#include "value.h"
#include "vm.h"

#define ALLOCATE_OBJ(type, objectType) (type*)allocateObject(sizeof(type), objectType)
#define ALLOCATE_OBJ_CST_SIZE(type, objectType, size) (type*)allocateObject(size, objectType)

static Obj* allocateObject(size_t size, ObjType type) {
  Obj* object = (Obj*)reallocate(NULL, 0, size);
  object->type = type;

  object->next = vm.objects;
  vm.objects = object;
  return object;
}

static ObjString* allocateString(char* chars, int length) {
  ObjString* string = ALLOCATE_OBJ_CST_SIZE(ObjString, OBJ_STRING, sizeof(ObjString) + length + 1);
  string->length = length;
  memcpy(string->chars, chars, length);
  string->chars[length] = '\0';
  return string;
}

ObjString* copyString(const char* chars, int length) {
  char* heapChars = ALLOCATE(char, length + 1);
  memcpy(heapChars, chars, length);
  heapChars[length] = '\0';
  return allocateString(heapChars, length);
}

ObjString* takeString(char* chars, int length) { 
    return allocateString(chars, length); 
}

void printObject(Value value) {
  switch (OBJ_TYPE(value)) {
    case OBJ_STRING:
      printf("%s", AS_CSTRING(value));
      break;
  }
}
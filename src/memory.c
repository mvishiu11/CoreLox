#include "memory.h"

#include <stdlib.h>

#include "vm.h"
#include "compiler.h"

#ifdef DEBUG_LOG_GC
#include <stdio.h>
#include "debug.h"
#endif

void* reallocate(void* pointer, size_t oldSize __attribute__((unused)), size_t newSize) {
  if (newSize > oldSize) {
#ifdef DEBUG_STRESS_GC
    collectGarbage();
#endif
  }

  if (newSize == 0) {
    free(pointer);
    return NULL;
  }

  void* result = realloc(pointer, newSize);
  if (result == NULL) exit(1);
  return result;
}

void markObject(Obj* object) {
  if (object == NULL) return;
  if (object->isMarked) return;

#ifdef DEBUG_LOG_GC
  printf("-- gc %p mark ", (void*)object);
  printValue(OBJ_VAL(object));
  printf("\n");
#endif

  object->isMarked = true;
  if (object->type == OBJ_STRING || object->type == OBJ_NATIVE) return;

  if (vm.grayCapacity < vm.grayCount + 1) {
    vm.grayCapacity = GROW_CAPACITY(vm.grayCapacity);
    vm.grayStack = (Obj**)realloc(vm.grayStack,
                                  sizeof(Obj*) * vm.grayCapacity);
  }

  vm.grayStack[vm.grayCount++] = object;
  if (vm.grayStack == NULL) exit(1);
}

void markValue(Value value) {
  if (IS_OBJ(value)) markObject(AS_OBJ(value));
}

static void markArray(ValueArray* array) {
  for (int i = 0; i < array->count; i++) {
    markValue(array->values[i]);
  }
}

static void blackenObject(Obj* object) {
#ifdef DEBUG_LOG_GC
  printf("-- gc %p blacken ", (void*)object);
  printValue(OBJ_VAL(object));
  printf("\n");
#endif

  switch (object->type) {
    case OBJ_CLOSURE: {
      ObjClosure* closure = (ObjClosure*)object;
      markObject((Obj*)closure->function);
      for (int i = 0; i < closure->upvalueCount; i++) {
        markObject((Obj*)closure->upvalues[i]);
      }
      break;
    }
    case OBJ_FUNCTION: {
      ObjFunction* function = (ObjFunction*)object;
      markObject((Obj*)function->name);
      markArray(&function->chunk.constants);
      break;
    }
    case OBJ_UPVALUE:
      markValue(((ObjUpvalue*)object)->closed);
      break;
    case OBJ_NATIVE:
    case OBJ_STRING:
      break;
  }
}

void freeObject(Obj* object) {
#ifdef DEBUG_LOG_GC
  printf("[MEM] %p free type %d\n", (void*)object, object->type);
#endif

  switch (object->type) {
    case OBJ_FUNCTION: {
      ObjFunction* function = (ObjFunction*)object;
      freeChunk(&function->chunk);
      FREE(ObjFunction, object);
      break;
    }
    case OBJ_NATIVE:
      FREE(ObjNative, object);
      break;
    case OBJ_CLOSURE: {
      ObjClosure* closure = (ObjClosure*)object;
      FREE_ARRAY(ObjUpvalue*, closure->upvalues, closure->upvalueCount);
      FREE(ObjClosure, object);
      break;
    }
    case OBJ_UPVALUE:
      FREE(ObjUpvalue, object);
      break;
    case OBJ_STRING: {
      FREE(ObjString, object);
      break;
    }
  }
}

static void markRoots() {
  // Mark the stack values
  for (Value* slot = vm.stack; slot < vm.stackTop; slot++) {
    markValue(*slot);
  }

  // Mark the closures
  for (int i = 0; i < vm.frameCount; i++) {
    markObject((Obj*)vm.frames[i].closure);
  }

  // Mark the open upvalues
  for (ObjUpvalue* upvalue = vm.openUpvalues;
       upvalue != NULL;
       upvalue = upvalue->next) {
    markObject((Obj*)upvalue);
  }

  // Mark the globals table
  markTable(&vm.globals);

  // Mark the compiler roots
  markCompilerRoots();
}

static void traceReferences() {
  while (vm.grayCount > 0) {
    Obj* object = vm.grayStack[--vm.grayCount];
    blackenObject(object);
  }
}

static void sweep() {
  Obj* previous = NULL;
  Obj* object = vm.objects;
  while (object != NULL) {
    if (object->isMarked) {
      object->isMarked = false;
      previous = object;
      object = object->next;
    } else {
      Obj* unreached = object;
      object = object->next;
      if (previous != NULL) {
        previous->next = object;
      } else {
        vm.objects = object;
      }

      freeObject(unreached);
    }
  }
}

void collectGarbage() {
#ifdef DEBUG_LOG_GC
  printf("-- gc begin\n");
#endif

  markRoots();
  traceReferences();
  tableRemoveWhite(&vm.strings);
  sweep();

#ifdef DEBUG_LOG_GC
  printf("-- gc end\n");
#endif
}

void freeObjectList(Obj* object) {
  while (object != NULL) {
    Obj* next = object->next;
    freeObject(object);
    object = next;
  }
}

void freeObjects() {
  Obj* object = vm.objects;
  while (object != NULL) {
    Obj* next = object->next;
    freeObject(object);
    object = next;
  }
}
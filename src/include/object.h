#ifndef corelox_object_h
#define corelox_object_h

#include "chunk.h"
#include "common.h"
#include "value.h"

/**
 * @file object.h
 * @brief Represents the object system and utility functions for managing objects.
 *
 * This header defines the `Obj` struct and its subtypes, which represent objects
 * in the virtual machine. Objects are managed by the garbage collector and can
 * represent values like strings and functions. The header also provides functions
 * for working with objects in the interpreter.
 */

/**
 * @brief Macro to check the type of an object.
 *
 * This macro checks the type of an object by extracting the `type` field from
 * the `Obj` struct. It is used to determine the type of an object when working
 * with objects in the interpreter.
 */
#define OBJ_TYPE(value) (AS_OBJ(value)->type)

/**
 * @brief Macro to check if an object is a function.
 *
 * This macro checks if an object is a function by comparing the type of the object
 * to the `OBJ_FUNCTION` type. It is used to determine if an object is a function when
 * working with objects in the interpreter.
 */
#define IS_FUNCTION(value) isObjType(value, OBJ_FUNCTION)

/**
 * @brief Macro to check if an object is a native function.
 *
 * This macro checks if an object is a native function by comparing the type of the object
 * to the `OBJ_NATIVE` type. It is used to determine if an object is a native function when
 * working with objects in the interpreter.
 */
#define IS_NATIVE(value) isObjType(value, OBJ_NATIVE)

/**
 * @brief Macro to check if an object is a closure.
 *
 * This macro checks if an object is a closure by comparing the type of the object
 * to the `OBJ_CLOSURE` type. It is used to determine if an object is a closure when
 * working with objects in the interpreter.
 */
#define IS_CLOSURE(value) isObjType(value, OBJ_CLOSURE)

/**
 * @brief Macro to check if an object is a string.
 *
 * This macro checks if an object is a string by comparing the type of the object
 * to the `OBJ_STRING` type. It is used to determine if an object is a string when
 * working with objects in the interpreter.
 */
#define IS_STRING(value) isObjType(value, OBJ_STRING)

/**
 * @brief Macro to cast a value to a function object.
 *
 * This macro casts a value to a function object by extracting the object pointer
 * from the `Value` struct and casting it to an `ObjFunction` pointer. It is used
 * to access the function object when working with function values in the interpreter.
 */
#define AS_FUNCTION(value) ((ObjFunction*)AS_OBJ(value))

/**
 * @brief Macro to cast a value to a native function object.
 *
 * This macro casts a value to a native function object by extracting the object pointer
 * from the `Value` struct and casting it to an `ObjNative` pointer. It is used to access
 * the native function object when working with native function values in the interpreter.
 */
#define AS_NATIVE(value) (((ObjNative*)AS_OBJ(value))->function)

/**
 * @brief Macro to cast a value to a native object.
 *
 * This macro casts a value to a native object by extracting the object pointer
 * from the `Value` struct and casting it to an `ObjNative` pointer. It is used
 * to access the native object when working with native values in the interpreter.
 */
#define AS_NATIVE_OBJ(value) ((ObjNative*)AS_OBJ(value))

/**
 * @brief Macro to cast a value to a closure object.
 *
 * This macro casts a value to a closure object by extracting the object pointer
 * from the `Value` struct and casting it to an `ObjClosure` pointer. It is used
 * to access the closure object when working with closure values in the interpreter.
 */
#define AS_CLOSURE(value) ((ObjClosure*)AS_OBJ(value))

/**
 * @brief Macro to cast a value to a string object.
 *
 * This macro casts a value to a string object by extracting the object pointer
 * from the `Value` struct and casting it to an `ObjString` pointer. It is used
 * to access the string object when working with string values in the interpreter.
 */
#define AS_STRING(value) ((ObjString*)AS_OBJ(value))

/**
 * @brief Macro to access the character data of a string object.
 *
 * This macro accesses the character data of a string object by casting the object
 * to an `ObjString` pointer and accessing the `chars` field. It is used to access
 * the character data of a string object when working with strings in the interpreter.
 */
#define AS_CSTRING(value) (((ObjString*)AS_OBJ(value))->chars)

/**
 * @brief Represents the different types of objects in the virtual machine.
 *
 * The `ObjType` enum represents the different types of objects that can be stored
 * in the virtual machine. This is used to distinguish between different types of
 * objects (e.g., strings, functions) when working with the interpreter.
 */
typedef enum {
  OBJ_FUNCTION,
  OBJ_NATIVE,
  OBJ_CLOSURE,
  OBJ_UPVALUE,
  OBJ_STRING,
} ObjType;

/**
 * @brief Represents the base object type in the virtual machine.
 *
 * The `Obj` struct represents the base object type in the virtual machine. It is
 * used to define the common fields shared by all object types, such as the object
 * type and a pointer to the next object in the linked list.
 */
struct Obj {
  ObjType type;
  struct Obj* next;
};

/**
 * @brief Represents a function object in the virtual machine.
 *
 * The `ObjFunction` struct represents a function object in the virtual machine, inheriting from
 * base object. It is used to store function values and manage the memory used to store the bytecode
 * instructions of the function. The struct includes the arity of the function, the chunk of
 * bytecode instructions, and the name of the function.
 *
 * Fields:
 *
 * - `obj`: The base object struct containing the object type and a pointer to the next object.
 * - `arity`: The number of arguments the function takes.
 * - `chunk`: The chunk of bytecode instructions for the function.
 * - `name`: The name of the function as a string object.
 */
typedef struct {
  Obj obj;
  int arity;
  int upvalueCount;
  Chunk chunk;
  ObjString* name;
} ObjFunction;

/**
 * @brief Represents a native function pointer type.
 *
 * The `NativeFn` type represents a pointer to a native function that can be called from the
 * virtual machine. It is used to define the signature of native functions that can be registered
 * with the virtual machine and called from Lox code.
 */
typedef Value (*NativeFn)(int argCount, Value* args);

/**
 * @brief Represents a native function object in the virtual machine.
 *
 * The `ObjNative` struct represents a native function object in the virtual machine, inheriting
 * from base object. It is used to store native function values and manage the memory used to store
 * the function pointer. The struct includes the function pointer to the native function.
 *
 * Fields:
 *
 * - `obj`: The base object struct containing the object type and a pointer to the next object.
 * - `function`: The function pointer to the native function.
 */
typedef struct {
  Obj obj;
  int arity;
  NativeFn function;
} ObjNative;

/**
 * @brief Represents an upvalue object in the virtual machine.
 *
 * The `ObjUpvalue` struct represents an upvalue object in the virtual machine, inheriting from base
 * object. It is used to store upvalue values and manage the memory used to store the location of
 * the captured variable. The struct includes the location of the captured variable.
 *
 * Fields:
 *
 * - `obj`: The base object struct containing the object type and a pointer to the next object.
 * - `location`: The location of the captured variable.
 */
typedef struct ObjUpvalue {
  Obj obj;
  Value* location;
  Value closed;
  struct ObjUpvalue* next;
} ObjUpvalue;

/**
 * @brief Represents a closure object in the virtual machine.
 *
 * The `ObjClosure` struct represents a closure object in the virtual machine, inheriting from base
 * object. It is used to store closure values and manage the memory used to store the function and
 * its captured environment. The struct includes the function object and the captured environment.
 *
 * Fields:
 *
 * - `obj`: The base object struct containing the object type and a pointer to the next object.
 * - `function`: The function object that the closure is created from.
 */
typedef struct {
  Obj obj;
  ObjFunction* function;
  ObjUpvalue** upvalues;
  int upvalueCount;
} ObjClosure;

/**
 * @brief Represents a string object in the virtual machine.
 *
 * The `ObjString` struct represents a string object in the virtual machine, inheriting from base
 * object. It is used to store string values and manage the memory used to store the characters of
 * the string. The struct includes the length of the string and a flexible array to store the
 * characters of the string.
 *
 * Fields:
 *
 * - `obj`: The base object struct containing the object type and a pointer to the next object.
 * - `length`: The length of the string.
 * - `hash`: The cache of hash value of the string for quick comparison, using FNV-1a hash
 * algorithm.
 * - `chars`: A flexible array to store the characters of the string.
 */
struct ObjString {
  Obj obj;
  int length;
  uint32_t hash;
  char chars[];
};

/**
 * @brief Creates a new function object.
 *
 * This function creates a new function object and initializes its fields with default values.
 * It allocates memory for the function object and initializes the bytecode
 * chunk with an empty chunk. The function object is used to store function values in the
 * virtual machine.
 *
 * @return The newly created function object as ObjFunction.
 */
ObjFunction* newFunction();

/**
 * @brief Creates a new native function object.
 *
 * This function creates a new native function object and initializes its fields with the given
 * function pointer. It allocates memory for the native function object and sets the function
 * pointer to the given native function. The native function object is used to store native
 * function values in the virtual machine.
 *
 * @param function The function pointer to the native function.
 * @return The newly created native function object as ObjNative.
 */
ObjNative* newNative(NativeFn function, int arity);

/**
 * @brief Creates a new closure object.
 *
 * This function creates a new closure object and initializes its fields with the given function.
 * It allocates memory for the closure object and sets the function to the given function object.
 * The closure object is used to store closure values in the virtual machine.
 *
 * @param function The function object to create the closure from.
 * @return The newly created closure object as ObjClosure.
 */
ObjClosure* newClosure(ObjFunction* function);

/**
 * @brief Creates a new upvalue object.
 *
 * This function creates a new upvalue object and initializes its fields with the given slot.
 * It allocates memory for the upvalue object and sets the slot to the given value pointer.
 * The upvalue object is used to store upvalue values in the virtual machine.
 *
 * @param slot The value pointer to the upvalue slot.
 * @return The newly created upvalue object as ObjUpvalue.
 */
ObjUpvalue* newUpvalue(Value* slot);

/**
 * @brief Creates a new string object from constant character data.
 *
 * This function creates a new string object from a constant character array
 * with the given length. It allocates memory for the string object and copies
 * the character data into the object's character array.
 *
 * @param chars The character data for the string.
 * @param length The length of the character data.
 * @return The newly created string object as ObjString.
 */
ObjString* copyString(const char* chars, int length);

/**
 * @brief Creates a new string object from a character array.
 *
 * This function creates a new string object from a character array with the
 * given length. It allocates memory for the string object and copies the
 * character data into the object's character array.
 *
 * @param chars The character data for the string.
 * @param length The length of the character data.
 * @return The newly created string object as ObjString.
 */
ObjString* takeString(char* chars, int length);

/**
 * @brief Prints an object to the standard output.
 *
 * This function prints an object to the console in a human-readable format.
 * It is used for debugging purposes or to inspect the objects stored in the
 * virtual machine during execution.
 *
 * @param value The value containing the object to print.
 */
void printObject(Value value);

/**
 * @brief Checks if an object is of a specific type.
 *
 * This function checks if an object is of a specific type by comparing the
 * type of the object to the given object type. It is used to determine the
 * type of an object when working with objects in the interpreter.
 *
 * @param value The value containing the object to check.
 * @param type The object type to compare against.
 * @return `true` if the object is of the given type, `false` otherwise.
 */
static inline bool isObjType(Value value, ObjType type) {
  return IS_OBJ(value) && AS_OBJ(value)->type == type;
}

#endif
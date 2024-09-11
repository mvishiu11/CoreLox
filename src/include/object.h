#ifndef corelox_object_h
#define corelox_object_h

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
 * @brief Macro to check if an object is a string.
 * 
 * This macro checks if an object is a string by comparing the type of the object
 * to the `OBJ_STRING` type. It is used to determine if an object is a string when
 * working with objects in the interpreter.
 */
#define IS_STRING(value) isObjType(value, OBJ_STRING)

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
 * @brief Represents a string object in the virtual machine.
 * 
 * The `ObjString` struct represents a string object in the virtual machine, inheriting from base object.
 * It is used to store string values and manage the memory used to store the characters
 * of the string. The struct includes the length of the string and a flexible array
 * to store the characters of the string.
 * 
 * Fields:
 * 
 * - `obj`: The base object struct containing the object type and a pointer to the next object.
 * - `length`: The length of the string.
 * - `hash`: The cache of hash value of the string for quick comparison, using FNV-1a hash algorithm.
 * - `chars`: A flexible array to store the characters of the string.
 */
struct ObjString {
  Obj obj;
  int length;
  uint32_t hash;
  char chars[];
};

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
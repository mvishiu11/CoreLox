#ifndef corelox_value_h
#define corelox_value_h

#include "common.h"

/**
 * @file value.h
 * @brief Represents the constant pool and utility functions for managing values.
 *
 * This header defines the `Value` type and provides structures and functions
 * for managing an array of values, which serves as the constant pool for the
 * bytecode chunk. The constant pool stores literals like numbers and strings
 * used during execution.
 */

/**
 * @brief Forward declaration of the `Obj` struct and its subtypes.
 *
 * The `Obj` struct is a forward declaration used to define a pointer to an
 * object in the virtual machine. It is used to represent objects like strings
 * and functions that are managed by the garbage collector.
 */
typedef struct Obj Obj;
typedef struct ObjString ObjString;

#ifdef NAN_BOXING

#include <string.h>

#define QNAN ((uint64_t)0x7ffc000000000000)
#define SIGN_BIT ((uint64_t)0x8000000000000000)

#define TAG_NIL 1    // 01.
#define TAG_FALSE 2  // 10.
#define TAG_TRUE 3   // 11.

typedef uint64_t Value;

#define NUMBER_VAL(num) numToValue(num)

#define OBJ_VAL(obj) (Value)(SIGN_BIT | QNAN | (uint64_t)(uintptr_t)(obj))

#define NIL_VAL ((Value)(uint64_t)(QNAN | TAG_NIL))

#define FALSE_VAL ((Value)(uint64_t)(QNAN | TAG_FALSE))

#define TRUE_VAL ((Value)(uint64_t)(QNAN | TAG_TRUE))

#define BOOL_VAL(b) ((b) ? TRUE_VAL : FALSE_VAL)

#define IS_NUMBER(value) (((value) & QNAN) != QNAN)

#define IS_OBJ(value) (((value) & (QNAN | SIGN_BIT)) == (QNAN | SIGN_BIT))

#define IS_BOOL(value) (((value) | 1) == TRUE_VAL)

#define IS_NIL(value) ((value) == NIL_VAL)

#define AS_NUMBER(value) valueToNum(value)

#define AS_OBJ(value) ((Obj*)(uintptr_t)((value) & ~(SIGN_BIT | QNAN)))

#define AS_BOOL(value) ((value) == TRUE_VAL)

static inline Value numToValue(double num) {
  Value value;
  memcpy(&value, &num, sizeof(double));
  return value;
}

static inline double valueToNum(Value value) {
  double num;
  memcpy(&num, &value, sizeof(Value));
  return num;
}

#else

/**
 * @brief Enumeration of value types in the virtual machine.
 *
 * The `ValueType` enum represents the different types of values that can be
 * stored in the virtual machine. This is used to distinguish between different
 * types of values (e.g., numbers, strings) when working with the interpreter.
 */
typedef enum { VAL_BOOL, VAL_NIL, VAL_NUMBER, VAL_OBJ } ValueType;

/**
 * @brief Represents a value in the virtual machine via a tagged union.
 *
 * The `Value` struct represents a value in the virtual machine, which can be
 * either a boolean or a number. It is used to store constants in the constant
 * pool and as the result of expressions during execution.
 */
typedef struct {
  ValueType type;
  union {
    bool boolean;
    double number;
    Obj* obj;
  } as;
} Value;

/**
 * @brief Checks if a `Value` is a boolean value.
 *
 * This macro checks if a `Value` struct represents a boolean value. It is used
 * to determine if a value is a boolean when working with values in the interpreter.
 */
#define IS_BOOL(value) ((value).type == VAL_BOOL)

/**
 * @brief Checks if a `Value` is a nil value.
 *
 * This macro checks if a `Value` struct represents a nil value. It is used to
 * determine if a value is nil when working with values in the interpreter.
 */
#define IS_NIL(value) ((value).type == VAL_NIL)

/**
 * @brief Checks if a `Value` is a number value.
 *
 * This macro checks if a `Value` struct represents a number value. It is used
 * to determine if a value is a number when working with values in the interpreter.
 */
#define IS_NUMBER(value) ((value).type == VAL_NUMBER)

/**
 * @brief Checks if a `Value` is an object value.
 *
 * This macro checks if a `Value` struct represents an object value. It is used
 * to determine if a value is an object when working with values in the interpreter.
 */
#define IS_OBJ(value) ((value).type == VAL_OBJ)

/**
 * @brief Accesses the boolean value of a `Value`.
 *
 * This macro extracts the boolean value from a `Value` struct. It is used to
 * access the boolean value stored in the tagged union, which is useful when
 * working with boolean values in the interpreter.
 */
#define AS_BOOL(value) ((value).as.boolean)

/**
 * @brief Accesses the number value of a `Value`.
 *
 * This macro extracts the number value from a `Value` struct. It is used to
 * access the number value stored in the tagged union, which is useful when
 * working with numeric values in the interpreter.
 */
#define AS_NUMBER(value) ((value).as.number)

/**
 * @brief Accesses the object value of a `Value`.
 *
 * This macro extracts the object value from a `Value` struct. It is used to
 * access the object value stored in the tagged union, which is useful when
 * working with object values in the interpreter.
 */
#define AS_OBJ(value) ((value).as.obj)

/**
 * @brief Creates a `Value` struct with a boolean value.
 *
 * This macro creates a `Value` struct with a boolean value. It is used to
 * create a `Value` struct that stores a boolean value, which is useful when
 * working with boolean values in the interpreter.
 */
#define BOOL_VAL(value) ((Value){VAL_BOOL, {.boolean = value}})

/**
 * @brief Creates a `Value` struct with a nil value.
 *
 * This macro creates a `Value` struct with a nil value. It is used to create
 * a `Value` struct that represents a nil value, which is useful when working
 * with nil values in the interpreter.
 */
#define NIL_VAL ((Value){VAL_NIL, {.number = 0}})

/**
 * @brief Creates a `Value` struct with a number value.
 *
 * This macro creates a `Value` struct with a number value. It is used to
 * create a `Value` struct that stores a numeric value, which is useful when
 * working with numeric values in the interpreter.
 */
#define NUMBER_VAL(value) ((Value){VAL_NUMBER, {.number = value}})

/**
 * @brief Creates a `Value` struct with an object value.
 *
 * This macro creates a `Value` struct with an object value. It is used to
 * create a `Value` struct that stores an object value, which is useful when
 * working with object values in the interpreter.
 */
#define OBJ_VAL(object) ((Value){VAL_OBJ, {.obj = (Obj*)object}})

#endif

/**
 * @brief Dynamic array for storing constant pool values.
 *
 * The `ValueArray` struct represents a dynamic array used to store constants
 * (e.g., numbers, strings) that are referenced by the bytecode instructions.
 * It dynamically grows as values are added, ensuring efficient memory management.
 */
typedef struct {
  int capacity;   ///< The allocated capacity of the array (in number of values).
  int count;      ///< The current number of values stored in the array.
  Value* values;  ///< Pointer to the dynamically allocated array of values.
} ValueArray;

/**
 * @brief Checks if two `Value` structs are equal.
 *
 * This function compares two `Value` structs for equality. It checks if the
 * two values are of the same type and have the same value. It is used to
 * compare values during execution to determine if they are equal.
 *
 * @param a The first `Value` to compare.
 * @param b The second `Value` to compare.
 * @return `true` if the values are equal, `false` otherwise.
 */
bool valuesEqual(Value a, Value b);

/**
 * @brief Initializes a `ValueArray` to an empty state.
 *
 * This function initializes a `ValueArray` with zero capacity and sets up
 * the necessary internal data structures for storing values. It prepares
 * the array to receive values as the constant pool grows.
 *
 * @param array Pointer to the `ValueArray` to initialize.
 */
void initValueArray(ValueArray* array);

/**
 * @brief Writes a value into the `ValueArray`.
 *
 * This function adds a value to the `ValueArray`, dynamically resizing
 * the array if necessary. The new value is appended to the end of the array.
 *
 * @param array Pointer to the `ValueArray` where the value is written.
 * @param value The `Value` to add to the array.
 */
void writeValueArray(ValueArray* array, Value value);

/**
 * @brief Frees the memory used by a `ValueArray`.
 *
 * This function deallocates the memory used by the `ValueArray` and resets
 * its state to empty. It is used to clean up memory after the array is no
 * longer needed.
 *
 * @param array Pointer to the `ValueArray` to free.
 */
void freeValueArray(ValueArray* array);

/**
 * @brief Prints a value to the standard output.
 *
 * This function prints a `Value` to the console in a human-readable format.
 * It is primarily used for debugging purposes or to inspect the values
 * stored in the constant pool.
 *
 * @param value The `Value` to print.
 */
void printValue(Value value);

#endif
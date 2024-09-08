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
 * @brief Represents a value in the virtual machine.
 *
 * The `Value` type is currently defined as a `double` to represent numeric
 * values (e.g., floating-point literals) that can be used in the interpreter.
 * This can be modified to support other types (e.g., strings, objects) as needed.
 */
typedef double Value;

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
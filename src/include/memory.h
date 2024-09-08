#ifndef corelox_memory_h
#define corelox_memory_h

#include "common.h"

/**
 * @file memory.h
 * @brief Memory management utilities for dynamic allocation.
 *
 * This header defines macros and functions for managing dynamic memory
 * allocation in the virtual machine. It provides utilities for growing
 * arrays, reallocating memory, and freeing dynamically allocated memory.
 */

/**
 * @brief Macro to determine the next capacity for a dynamically growing array.
 *
 * This macro doubles the capacity of an array unless the current capacity
 * is less than 8, in which case it increases the capacity to 8. This ensures
 * that small arrays grow quickly and larger arrays double in size to
 * optimize memory allocation.
 *
 * @param capacity The current capacity of the array.
 * @return The new capacity, which is either doubled or set to 8.
 */
#define GROW_CAPACITY(capacity) ((capacity) < 8 ? 8 : (capacity) * 2)

/**
 * @brief Macro to grow a dynamically allocated array.
 *
 * This macro reallocates memory for an array of a given type, increasing its size
 * from `oldCount` elements to `newCount` elements. The `reallocate()` function
 * handles the actual memory allocation and copying of data.
 *
 * @param type The data type of the elements in the array.
 * @param pointer A pointer to the current array.
 * @param oldCount The current number of elements in the array.
 * @param newCount The new number of elements the array should hold.
 * @return A pointer to the newly reallocated array with the increased capacity.
 */
#define GROW_ARRAY(type, pointer, oldCount, newCount) \
  (type*)reallocate(pointer, sizeof(type) * (oldCount), sizeof(type) * (newCount))

/**
 * @brief Macro to free a dynamically allocated array.
 *
 * This macro frees memory for an array by calling the `reallocate()` function
 * with a new size of 0. It effectively deallocates the array and returns its
 * memory back to the system.
 *
 * @param type The data type of the elements in the array.
 * @param pointer A pointer to the array to free.
 * @param oldCount The current number of elements in the array.
 */
#define FREE_ARRAY(type, pointer, oldCount) reallocate(pointer, sizeof(type) * (oldCount), 0)

/**
 * @brief Reallocates a block of memory.
 *
 * This function dynamically resizes a block of memory. It can be used to allocate
 * new memory, grow or shrink existing memory, or free the memory by setting the
 * new size to 0. It is a central utility for dynamic memory management in the virtual
 * machine and compiler.
 *
 * @param pointer A pointer to the current memory block, or NULL if allocating new memory.
 * @param oldSize The current size of the memory block in bytes.
 * @param newSize The new size of the memory block in bytes. If 0, the memory is freed.
 * @return A pointer to the newly allocated memory block, or NULL if the memory was freed.
 */
void* reallocate(void* pointer, size_t oldSize, size_t newSize);

#endif
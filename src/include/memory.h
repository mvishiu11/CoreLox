#ifndef corelox_memory_h
#define corelox_memory_h

#include "common.h"
#include "object.h"

/**
 * @file memory.h
 * @brief Memory management utilities for dynamic allocation.
 *
 * This header defines macros and functions for managing dynamic memory
 * allocation in the virtual machine. It provides utilities for growing
 * arrays, reallocating memory, and freeing dynamically allocated memory.
 */

/**
 * @brief Allocates a block of memory for a given type.
 *
 * This macro allocates a block of memory for a given type and count.
 * It is used to allocate memory for arrays and other data structures
 * that require dynamic memory allocation.
 *
 * @param type The data type of the elements in the array.
 * @param count The number of elements to allocate.
 * @return A pointer to the newly allocated memory block.
 */
#define ALLOCATE(type, count) (type*)reallocate(NULL, 0, sizeof(type) * (count))

/**
 * @brief Frees a block of memory.
 *
 * This macro frees a block of memory allocated by the `ALLOCATE()` macro.
 * It is used to deallocate memory for arrays and other data structures
 * that are no longer needed.
 *
 * @param type The data type of the elements in the array.
 * @param pointer A pointer to the memory block to free.
 */
#define FREE(type, pointer) reallocate(pointer, sizeof(type), 0)

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

/**
 * @brief Frees an object allocated by the virtual machine.
 *
 * This function frees an object allocated by the virtual machine. It is used
 * to deallocate memory for objects like strings and functions that are managed
 * by the garbage collector.
 *
 * @param object A pointer to the object to free.
 */
void freeObject(Obj* object);

/**
 * @brief Initializes the garbage collector.
 *
 * This function initializes the garbage collector by setting the initial
 * garbage collection threshold and allocating memory for the object list.
 */
void collectGarbage();

/**
 * @brief Marks an object as reachable during garbage collection.
 *
 * This function marks an object as reachable during garbage collection. It is
 * used to traverse the object graph and mark all objects that are still in use
 * by the virtual machine.
 *
 * @param object A pointer to the object to mark.
 */
void markValue(Value value);

/**
 * @brief Marks an object as reachable during garbage collection.
 *
 * This function marks an object as reachable during garbage collection. It is
 * used to traverse the object graph and mark all objects that are still in use
 * by the virtual machine.
 *
 * @param object A pointer to the object to mark.
 */
void markObject(Obj* object);

/**
 * @brief Frees all objects in the passed object list.
 *
 * This function frees all objects in the passed object list. It is used to
 * deallocate memory for objects created during execution that are no longer
 * needed.
 */
void freeObjectList(Obj* object);

/**
 * @brief Frees all objects managed by the virtual machine.
 *
 * This function frees all objects managed by the virtual machine. It is called
 * when the virtual machine is shut down to release all dynamically allocated
 * memory used by objects created during execution.
 */
void freeObjects();

#endif
#ifndef corelox_line_info_h
#define corelox_line_info_h

#include <stdio.h>
#include <stdlib.h>

#include "common.h"

/**
 * @file line_info.h
 * @brief Run-length encoding for tracking source line information in bytecode.
 *
 * This header defines data structures and functions for managing
 * line information in a chunk of bytecode. The line information
 * is stored using run-length encoding (RLE) to efficiently map
 * bytecode instructions to their corresponding source lines.
 */

/**
 * @typedef Value
 * @brief Placeholder typedef for a value type (e.g., double).
 *
 * This type alias is a placeholder for values stored in bytecode
 * chunks. In this case, it is defined as a `double`, but it can
 * be changed as needed.
 */
typedef double Value;

/**
 * @brief Represents a run-length encoded entry for line information.
 *
 * The `LineInfo` struct stores the line number and the run length
 * (i.e., how many consecutive bytecode instructions correspond to
 * that line). This helps efficiently encode line information without
 * storing the line number for every single bytecode instruction.
 */
typedef struct {
  int line;        ///< The source line number.
  int run_length;  ///< Number of consecutive instructions from this line.
} LineInfo;

/**
 * @brief Dynamic array for storing `LineInfo` entries.
 *
 * The `LineInfoArray` struct is a dynamically resizable array
 * used to store run-length encoded line information for a chunk
 * of bytecode.
 */
typedef struct {
  int count;        ///< Number of `LineInfo` entries currently in use.
  int capacity;     ///< Allocated capacity for the array.
  LineInfo* lines;  ///< Pointer to the array of `LineInfo` entries.
} LineInfoArray;

/**
 * @brief Initializes a `LineInfoArray`.
 *
 * This function initializes a `LineInfoArray` to an empty state
 * with zero capacity and no allocated memory. It sets up the data
 * structure for later use.
 *
 * @param array Pointer to the `LineInfoArray` to initialize.
 */
void initLineInfoArray(LineInfoArray* array);

/**
 * @brief Frees the memory used by a `LineInfoArray`.
 *
 * This function deallocates all memory used by the `LineInfoArray`
 * and resets it to an empty state. It ensures that no memory leaks
 * occur after the array is no longer needed.
 *
 * @param array Pointer to the `LineInfoArray` to free.
 */
void freeLineInfoArray(LineInfoArray* array);

/**
 * @brief Prints a single `LineInfo` entry.
 *
 * This function prints the line number and run length of a `LineInfo`
 * entry in a human-readable format. It is mainly used for debugging
 * and verifying the correctness of line information.
 *
 * @param value The `LineInfo` entry to print.
 */
void printLineInfo(LineInfo value);

#endif
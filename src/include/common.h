#ifndef corelox_common_h
#define corelox_common_h

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

/**
 * @file common.h
 * @brief Common definitions and includes used throughout the project.
 *
 * This header file contains common utility definitions and standard
 * includes that are shared across various components of the virtual
 * machine and the compiler. It ensures consistent inclusion of types
 * like `bool`, `size_t`, and `uint8_t` throughout the project.
 */

/*
 * Include standard libraries for boolean types, size types,
 * and fixed-width integer types, ensuring portability.
 */
#include <stdbool.h>  ///< Provides `bool` type for boolean values.
#include <stddef.h>   ///< Provides `size_t` and other standard definitions.
#include <stdint.h>   ///< Provides fixed-width integer types (e.g., `uint8_t`, `uint16_t`).

/**
 * @brief Enables bytecode disassembly and debugging features.
 *
 * Defining `DEBUG_PRINT_CODE` enables the disassembly of the generated
 * bytecode after compilation, which can help with debugging and verifying
 * the correctness of the code. This flag is typically used during development.
 */
#define DEBUG_PRINT_CODE

/**
 * @brief Enables detailed tracing of bytecode execution.
 *
 * Defining `DEBUG_TRACE_EXECUTION` enables detailed tracing of the
 * execution of the bytecode in the virtual machine. Each instruction
 * is printed along with the stack state after it executes. This is
 * useful for debugging the execution flow of the VM.
 */
//#define DEBUG_TRACE_EXECUTION

/**
 * @brief Enables garbage collection logging.
 *
 * Defining `DEBUG_LOG_GC` enables logging of garbage collection
 * events, including when the garbage collector is run and the
 * number of bytes allocated and freed. This can help track memory
 * usage and identify memory leaks in the virtual machine.
 */
//#define DEBUG_LOG_GC

/**
 * @brief Enables garbage collection stress testing.
 *
 * Defining `DEBUG_STRESS_GC` enables stress testing of the garbage
 * collector by running it after every allocation. This can help
 * identify issues with the garbage collector and memory management.
 * This flag is typically used during development and testing.
 */
//#define DEBUG_STRESS_GC

// hard limit on the number of local variables in a function
#define UINT8_COUNT (UINT8_MAX + 1)

#endif

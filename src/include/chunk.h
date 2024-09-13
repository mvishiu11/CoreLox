#ifndef corelox_chunk_h
#define corelox_chunk_h

#include "common.h"
#include "line_info.h"
#include "value.h"

/**
 * @brief Represents the operation codes (opcodes) for the virtual machine.
 *
 * These opcodes represent different bytecode instructions that the virtual
 * machine can execute, such as basic arithmetic operations and control flow
 * management.
 */
typedef enum {
  OP_CONSTANT,       ///< Push a constant onto the stack.
  OP_CONSTANT_LONG,  ///< Push a large constant (24-bit index) onto the stack.
  OP_NIL,            ///< Push a nil value onto the stack.
  OP_TRUE,           ///< Push a true value onto the stack.
  OP_FALSE,          ///< Push a false value onto the stack.
  OP_POP,            ///< Pop the top value from the stack.
  OP_SET_LOCAL,      ///< Set a local variable.
  OP_SET_GLOBAL,     ///< Set a global variable.
  OP_GET_LOCAL,      ///< Get a local variable.
  OP_GET_GLOBAL,     ///< Get a global variable.
  OP_DEFINE_GLOBAL,  ///< Define a global variable.
  OP_EQUAL,          ///< Check if two values are equal.
  OP_GREATER,        ///< Check if one value is greater than another.
  OP_LESS,           ///< Check if one value is less than another.
  OP_ADD,            ///< Add two values.
  OP_SUBTRACT,       ///< Subtract one value from another.
  OP_MULTIPLY,       ///< Multiply two values.
  OP_DIVIDE,         ///< Divide one value by another.
  OP_MODULO,         ///< Compute the remainder of a division.
  OP_NOT,            ///< Negate a boolean value.
  OP_NEGATE,         ///< Negate a value.
  OP_PRINT,          ///< Print the top value on the stack.
  OP_JUMP,           ///< Jump to a different location in the bytecode.s
  OP_JUMP_IF_FALSE,  ///< Jump if the top value on the stack is false.
  OP_JUMP_IF_TRUE,   ///< Jump if the top value on the stack is true.
  OP_LOOP,           ///< Loop back to a previous location in the bytecode.
  OP_RETURN,         ///< Return from the current function or program.
} OpCode;

/**
 * @brief Represents a chunk of bytecode and its associated metadata.
 *
 * A `Chunk` holds the bytecode for a function or a script, along with
 * additional data like the source lines for debugging and constants
 * used in the bytecode. It dynamically grows as more instructions are added.
 */
typedef struct {
  int count;             ///< Number of instructions (bytes) in the chunk.
  int capacity;          ///< Allocated capacity for the bytecode (in bytes).
  uint8_t* code;         ///< Pointer to the array of bytecode instructions.
  LineInfoArray lines;   ///< Array mapping instructions to their source lines.
  ValueArray constants;  ///< Array of constants used in the chunk.
} Chunk;

/**
 * @brief Initializes a chunk.
 *
 * This function initializes an empty `Chunk` with zero instructions
 * and no allocated capacity. It sets up the internal data structures
 * needed for storing bytecode, constants, and source line information.
 *
 * @param chunk Pointer to the chunk to initialize.
 */
void initChunk(Chunk* chunk);

/**
 * @brief Frees the memory used by a chunk.
 *
 * This function deallocates all the memory used by the `Chunk`,
 * including its bytecode, constants, and line information. It resets
 * the chunk back to its initial, uninitialized state.
 *
 * @param chunk Pointer to the chunk to free.
 */
void freeChunk(Chunk* chunk);

/**
 * @brief Writes a byte of bytecode into the chunk.
 *
 * This function adds a single byte of bytecode into the `Chunk`. It
 * also records the source line associated with that byte for debugging purposes.
 *
 * @param chunk Pointer to the chunk where the byte is written.
 * @param byte The byte to write.
 * @param line The source line where this byte originated.
 */
void writeChunk(Chunk* chunk, uint8_t byte, int line);

/**
 * @brief Writes a constant value into the chunk.
 *
 * This function adds a constant value to the chunk’s constant pool
 * and emits the corresponding bytecode instruction to push that
 * constant onto the stack.
 *
 * @param chunk Pointer to the chunk where the constant is written.
 * @param value The constant value to write.
 * @param line The source line where this constant was used.
 * @return The index of the constant in the constant array.
 */
int writeConstant(Chunk* chunk, Value value, int line);

/**
 * @brief Adds a constant to the chunk's constant pool.
 *
 * This function adds a constant value to the chunk's constant array
 * and returns its index. The constant can later be accessed via
 * bytecode instructions like `OP_CONSTANT`.
 *
 * @param chunk Pointer to the chunk where the constant will be added.
 * @param value The constant value to add.
 * @return The index of the constant in the constant array.
 */
int addConstant(Chunk* chunk, Value value);

/**
 * @brief Retrieves the source line for a given bytecode instruction.
 *
 * Given the index of an instruction in the chunk's bytecode, this
 * function returns the source line where the instruction originated.
 * It is primarily used for debugging and error reporting.
 *
 * @param chunk Pointer to the chunk containing the instruction.
 * @param instructionIndex The index of the instruction in the bytecode.
 * @return The source line number for the instruction.
 */
int getLine(Chunk* chunk, int instructionIndex);

#endif
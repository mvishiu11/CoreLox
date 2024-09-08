#ifndef corelox_debug_h
#define corelox_debug_h

#include "chunk.h"

/**
 * @file debug.h
 * @brief Functions for disassembling and debugging bytecode.
 *
 * This header defines utility functions for disassembling and printing
 * human-readable representations of bytecode chunks. These functions
 * are used to help with debugging by providing a detailed view of
 * the bytecode instructions generated during compilation.
 */

/**
 * @brief Disassembles the entire chunk of bytecode.
 *
 * This function prints a human-readable disassembly of the bytecode
 * in the given `Chunk`. Each instruction in the chunk is printed
 * along with its bytecode offset and, if applicable, any constants
 * associated with the instruction.
 *
 * @param chunk Pointer to the chunk of bytecode to disassemble.
 * @param name A label or name for the chunk (e.g., function name or script name).
 */
void disassembleChunk(Chunk* chunk, const char* name);

/**
 * @brief Disassembles a single instruction in a chunk.
 *
 * This function prints a human-readable representation of a single
 * bytecode instruction at the given offset within a chunk. The disassembly
 * includes the bytecode for the instruction, the offset, and any constants
 * or values associated with the instruction.
 *
 * @param chunk Pointer to the chunk containing the instruction.
 * @param offset The byte offset within the chunk where the instruction begins.
 * @return The offset of the next instruction in the chunk.
 */
int disassembleInstruction(Chunk* chunk, int offset);

#endif
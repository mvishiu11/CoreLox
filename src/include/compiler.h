#ifndef carbonlox_compiler_h
#define carbonlox_compiler_h

#include "scanner.h"
#include "vm.h"

/**
 * @brief Holds the parser state during compilation.
 *
 * The `Parser` struct contains the current and previous tokens,
 * along with error handling flags (`hadError` and `panicMode`).
 * The `panicMode` flag is used to suppress further error reporting
 * until recovery after a critical error.
 */
typedef struct {
  Token current;   ///< The current token being parsed.
  Token previous;  ///< The previously parsed token.
  bool hadError;   ///< Indicates if an error occurred during compilation.
  bool panicMode;  ///< Enables panic mode to suppress cascading errors.
} Parser;

/**
 * @brief Defines operator precedence levels for parsing.
 *
 * The `Precedence` enum defines the precedence for different
 * classes of operators, from `PREC_NONE` (lowest) to `PREC_PRIMARY`
 * (highest). Precedence levels are used to determine the order in
 * which expressions are evaluated.
 */
typedef enum {
  PREC_NONE,        ///< No precedence.
  PREC_ASSIGNMENT,  ///< Assignment (`=`).
  PREC_OR,          ///< Logical OR (`or`).
  PREC_AND,         ///< Logical AND (`and`).
  PREC_EQUALITY,    ///< Equality operators (`==`, `!=`).
  PREC_COMPARISON,  ///< Comparison operators (`<`, `>`, `<=`, `>=`).
  PREC_TERM,        ///< Term operators (`+`, `-`).
  PREC_FACTOR,      ///< Factor operators (`*`, `/`).
  PREC_UNARY,       ///< Unary operators (`!`, `-`).
  PREC_CALL,        ///< Call expressions (`.`, `()`).
  PREC_PRIMARY      ///< Primary expressions (identifiers, literals).
} Precedence;

/**
 * @brief Function pointer type for parsing expressions.
 *
 * `ParseFn` is a type alias for a function pointer that represents
 * a parsing function. Each function parses a specific type of expression
 * based on its prefix or infix position in the source code.
 */
typedef void (*ParseFn)();

/**
 * @brief Defines a rule for parsing a specific token.
 *
 * The `ParseRule` struct maps a token to its corresponding prefix
 * and infix parsing functions, along with its precedence level.
 * It is used to handle different parsing strategies for operators
 * and other tokens.
 */
typedef struct {
  ParseFn prefix;         ///< The function to parse this token as a prefix expression.
  ParseFn infix;          ///< The function to parse this token as an infix expression.
  Precedence precedence;  ///< The precedence level of this token.
} ParseRule;

/**
 * @brief Compiles the source code into bytecode.
 *
 * This function serves as the entry point for the compiler. It takes
 * the source code as input, tokenizes it, and generates the corresponding
 * bytecode. The compiled bytecode is written into the provided chunk.
 *
 * @param source The source code to compile.
 * @param chunk The chunk where the generated bytecode is stored.
 * @return true if the compilation was successful, false if any errors occurred.
 */
bool compile(const char* source, Chunk* chunk);

#endif

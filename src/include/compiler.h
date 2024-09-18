#ifndef corelox_compiler_h
#define corelox_compiler_h

#include "jump_list.h"
#include "object.h"
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
  PREC_TERNARY,     ///< Ternary conditional (`? :`).
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
 *
 * @param canAssign Indicates if the expression can be assigned to.
 */
typedef void (*ParseFn)(bool canAssign);

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
 * @brief Struct for storing local variable information.
 *
 * The `Local` struct represents a local variable in the compiler.
 * It holds the name of the variable and its depth in the scope stack.
 * Local variables are used to track variables in the source code.
 */
typedef struct {
  Token name;
  int depth;
  bool isCaptured;
} Local;

/**
 * @brief Struct defining upvalue information.
 *
 * The `Upvalue` struct represents an upvalue in the compiler. It holds
 * the index of the upvalue and a flag indicating whether it is a local
 * or non-local upvalue. Upvalues are used to capture variables from
 * enclosing scopes in closures.
 */
typedef struct {
  uint8_t index;
  bool isLocal;
} Upvalue;

/**
 * @brief Struct defining type of function being compiled.
 *
 * The `FunctionType` enum defines the type of function being compiled.
 * It is used to distinguish between top-level script code and function
 * bodies during compilation.
 */
typedef enum { 
  TYPE_METHOD,
  TYPE_FUNCTION, 
  TYPE_SCRIPT 
} FunctionType;

/**
 * @brief Struct for storing compiler state during compilation.
 * 
 * The `Compiler` struct holds the state of the compiler during compilation.
 * It tracks the current function being compiled, the local variables in scope,
 * and the upvalues captured by the function. The struct also manages the
 * scope depth and loop state during compilation.
 * 
 * @tparam enclosing The enclosing compiler, if any.
 * @tparam function The function being compiled.
 * @tparam type The type of function being compiled.
 * 
 * @tparam locals Array of local variables in the current scope.
 * @tparam localCapacity The allocated capacity for local variables.
 * @tparam localCount The number of local variables in the current scope.
 * @tparam scopeDepth The current depth of the scope stack.
 * 
 * @tparam upvalues Array of upvalues captured by the function.
 * @tparam upvalueCapacity The allocated capacity for upvalues.
 * @tparam upvalueCount The number of upvalues captured by the function.
 * 
 * @tparam currentLoopStart The starting offset of the current loop.
 * @tparam currentLoopEnd The ending offset of the current loop.
 * @tparam currentLoopDepth The current depth of the loop stack.
 * @tparam breakJumps List of break jump offsets for the current loop.
 */
typedef struct Compiler {
  struct Compiler* enclosing;
  ObjFunction* function;
  FunctionType type;

  Local* locals;
  int localCapacity;
  int localCount;
  int scopeDepth;

  Upvalue* upvalues;
  int upvalueCapacity;
  int upvalueCount;

  int currentLoopStart;
  int currentLoopEnd;
  int currentLoopDepth;
  JumpList breakJumps;
} Compiler;

/**
 * @brief Struct for storing class compiler state during compilation.
 * 
 * The `ClassCompiler` struct holds the state of the class compiler during
 * compilation. It tracks the enclosing class compiler, if any, and is used
 * to manage the state of class declarations and methods during compilation.
 * 
 * @tparam enclosing The enclosing class compiler, if any.
 */
typedef struct ClassCompiler {
  struct ClassCompiler* enclosing;
} ClassCompiler;

/**
 * @brief Compiles the source code into bytecode.
 *
 * This function compiles the source code into a chunk of bytecode
 * that can be executed by the virtual machine. It initializes the
 * parser and compiler state, and returns the compiled function.
 *
 * @param source The source code to compile.
 * @return The compiled function as `ObjFunction`.
 */
ObjFunction* compile(const char* source);

/**
 * @brief Marks all objects used during compilation as reachable by garbage collection
 *
 * This function marks all objects used during compilation as reachable by the garbage collector.
 * It is used to ensure that objects created during compilation are not prematurely collected.
 */
void markCompilerRoots();

#endif

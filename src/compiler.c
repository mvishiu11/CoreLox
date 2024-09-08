#include "compiler.h"

#include <stdio.h>
#include <stdlib.h>

#include "common.h"

#ifdef DEBUG_PRINT_CODE
#include "debug.h"
#endif

// Global parser state.
Parser parser;
Chunk* compilingChunk;

// Retrieves the current chunk being compiled.
static Chunk* currentChunk() { return compilingChunk; }

// Error Reporting Functions

/**
 * @brief Reports an error at a specific token.
 *
 * This function outputs an error message for a given token.
 * It handles panic mode to ensure that error reporting
 * does not cascade and suppresses further errors until
 * recovery.
 *
 * @param token The token where the error occurred.
 * @param message The error message to display.
 */
static void errorAt(Token* token, const char* message) {
  if (parser.panicMode) return;  // Suppress errors in panic mode.
  parser.panicMode = true;       // Enter panic mode.

  fprintf(stderr, "[line %d] Error", token->line);

  if (token->type == TOKEN_EOF) {
    fprintf(stderr, " at end");
  } else if (token->type != TOKEN_ERROR) {
    fprintf(stderr, " at '%.*s'", token->length, token->start);
  }

  fprintf(stderr, ": %s\n", message);
  parser.hadError = true;  // Mark that an error occurred.
}

/**
 * @brief Reports an error at the current token.
 *
 * A convenience function that reports an error at the current token.
 *
 * @param message The error message to display.
 */
static void errorAtCurrent(const char* message) { errorAt(&parser.current, message); }

/**
 * @brief Reports an error at the previous token.
 *
 * A convenience function that reports an error at the previous token.
 *
 * @param message The error message to display.
 */
static void error(const char* message) { errorAt(&parser.previous, message); }

// Helper Functions for Token Parsing

/**
 * @brief Advances the parser to the next token.
 *
 * This function moves the parser to the next token in the
 * token stream, skipping over any error tokens. It sets
 * the `previous` token to the current one, then reads
 * the next token.
 */
static void advance() {
  parser.previous = parser.current;

  for (;;) {
    parser.current = scanToken();                   // Scan the next token.
    if (parser.current.type != TOKEN_ERROR) break;  // Skip errors.
    errorAtCurrent(parser.current.start);           // Report the error.
  }
}

/**
 * @brief Consumes a token of a specific type, advancing the parser.
 *
 * If the current token matches the expected `type`, the parser
 * advances. Otherwise, an error message is reported.
 *
 * @param type The expected token type.
 * @param message The error message to report if the type does not match.
 */
static void consume(TokenType type, const char* message) {
  if (parser.current.type == type) {
    advance();  // Token matches, advance to the next token.
    return;
  }
  errorAtCurrent(message);  // Report the error if the token does not match.
}

// Bytecode Emission Functions

/**
 * @brief Emits a single byte of bytecode.
 *
 * This function writes a byte of bytecode into the current chunk.
 *
 * @param byte The byte to emit.
 */
static void emitByte(uint8_t byte) { writeChunk(currentChunk(), byte, parser.previous.line); }

/**
 * @brief Emits two consecutive bytes of bytecode.
 *
 * This function emits two bytes of bytecode in sequence.
 *
 * @param byte1 The first byte.
 * @param byte2 The second byte.
 */
static void emitBytes(uint8_t byte1, uint8_t byte2) {
  emitByte(byte1);
  emitByte(byte2);
}

/**
 * @brief Emits a constant value as bytecode.
 *
 * Writes the constant value to the chunk and emits the appropriate
 * constant instruction for it.
 *
 * @param value The constant value to emit.
 */
static void emitConstant(Value value) {
  writeConstant(currentChunk(), value, parser.previous.line);
}

/**
 * @brief Emits a return instruction.
 *
 * This function emits a return instruction (`OP_RETURN`)
 * to mark the end of the function or expression.
 */
static void emitReturn() { emitByte(OP_RETURN); }

/**
 * @brief Finalizes the compilation process.
 *
 * This function emits the return instruction to complete the
 * compilation process and optionally disassembles the chunk
 * if debugging is enabled.
 */
static void endCompiler() {
  emitReturn();
#ifdef DEBUG_PRINT_CODE
  if (!parser.hadError) {
    disassembleChunk(currentChunk(), "code");
  }
#endif
}

// Parsing Expression Functions

// Forward declarations

static void expression();
static void parsePrecedence(Precedence precedence);
static ParseRule* getRule(TokenType type);

/**
 * @brief Parses an expression.
 *
 * This is the top-level expression parsing function. It parses
 * an expression using the precedence of assignment.
 */
static void expression() { parsePrecedence(PREC_ASSIGNMENT); }

/**
 * @brief Parses a binary operation.
 *
 * This function parses a binary operator (e.g., `+`, `-`, `*`, `/`)
 * and the right-hand side of the expression.
 */
static void binary() {
  TokenType operatorType = parser.previous.type;
  ParseRule* rule = getRule(operatorType);
  parsePrecedence((Precedence)(rule->precedence + 1));  // Parse the RHS with higher precedence.

  switch (operatorType) {
    case TOKEN_BANG_EQUAL:
      emitBytes(OP_EQUAL, OP_NOT);
      break;
    case TOKEN_EQUAL_EQUAL:
      emitByte(OP_EQUAL);
      break;
    case TOKEN_GREATER:
      emitByte(OP_GREATER);
      break;
    case TOKEN_GREATER_EQUAL:
      emitBytes(OP_LESS, OP_NOT);
      break;
    case TOKEN_LESS:
      emitByte(OP_LESS);
      break;
    case TOKEN_LESS_EQUAL:
      emitBytes(OP_GREATER, OP_NOT);
      break;
    case TOKEN_PLUS:
      emitByte(OP_ADD);
      break;
    case TOKEN_MINUS:
      emitByte(OP_SUBTRACT);
      break;
    case TOKEN_STAR:
      emitByte(OP_MULTIPLY);
      break;
    case TOKEN_SLASH:
      emitByte(OP_DIVIDE);
      break;
    default:
      return;  // Unreachable.
  }
}

/**
 * @brief Parses a literal value.
 *
 * This function emits the appropriate bytecode for a literal value.
 */
static void literal() {
  switch (parser.previous.type) {
    case TOKEN_FALSE:
      emitByte(OP_FALSE);
      break;
    case TOKEN_NIL:
      emitByte(OP_NIL);
      break;
    case TOKEN_TRUE:
      emitByte(OP_TRUE);
      break;
    default:
      return;  // Unreachable.
  }
}

/**
 * @brief Parses a grouping expression.
 *
 * This function parses expressions enclosed in parentheses,
 * ensuring the parentheses are balanced.
 */
static void grouping() {
  expression();
  consume(TOKEN_RIGHT_PAREN, "Expect ')' after expression.");
}

/**
 * @brief Parses a numeric literal.
 *
 * This function parses a number token and emits the corresponding
 * bytecode for the constant.
 */
static void number() {
  double value = strtod(parser.previous.start, NULL);
  emitConstant(NUMBER_VAL(value));
}

/**
 * @brief Parses a unary operation.
 *
 * This function parses unary operators (e.g., `-`) and their
 * corresponding operand.
 */
static void unary() {
  TokenType operatorType = parser.previous.type;
  parsePrecedence(PREC_UNARY);  // Compile the operand.

  // Emit the appropriate bytecode for the operator.
  switch (operatorType) {
    case TOKEN_BANG:
      emitByte(OP_NOT);
      break;
    case TOKEN_MINUS:
      emitByte(OP_NEGATE);
      break;
    default:
      return;  // Unreachable.
  }
}

/**
 * @brief Parses an expression based on operator precedence.
 *
 * This function parses expressions using a precedence climbing
 * algorithm, handling operators based on their precedence levels.
 *
 * @param precedence The precedence of the current operator.
 */
static void parsePrecedence(Precedence precedence) {
  advance();  // Move to the next token.
  ParseFn prefixRule = getRule(parser.previous.type)->prefix;
  if (prefixRule == NULL) {
    error("Expect expression.");
    return;
  }

  prefixRule();  // Call the prefix function.

  while (precedence <= getRule(parser.current.type)->precedence) {
    advance();  // Move to the next token.
    ParseFn infixRule = getRule(parser.previous.type)->infix;
    infixRule();  // Call the infix function.
  }
}

// Parse Rules and Operator Precedence Table

// clang-format off

ParseRule rules[] = {
    [TOKEN_AND]           = {NULL,     NULL,   PREC_NONE},
    [TOKEN_BANG]          = {unary,    NULL,   PREC_NONE},
    [TOKEN_BANG_EQUAL]    = {NULL,     binary, PREC_EQUALITY},
    [TOKEN_CLASS]         = {NULL,     NULL,   PREC_NONE},
    [TOKEN_COMMA]         = {NULL,     NULL,   PREC_NONE},
    [TOKEN_DOT]           = {NULL,     NULL,   PREC_NONE},
    [TOKEN_ELSE]          = {NULL,     NULL,   PREC_NONE},
    [TOKEN_EQUAL]         = {NULL,     NULL,   PREC_NONE},
    [TOKEN_EQUAL_EQUAL]   = {NULL,     binary, PREC_EQUALITY},
    [TOKEN_EOF]           = {NULL,     NULL,   PREC_NONE},
    [TOKEN_ERROR]         = {NULL,     NULL,   PREC_NONE},
    [TOKEN_FALSE]         = {literal,  NULL,   PREC_NONE},
    [TOKEN_FOR]           = {NULL,     NULL,   PREC_NONE},
    [TOKEN_FUN]           = {NULL,     NULL,   PREC_NONE},
    [TOKEN_GREATER]       = {NULL,     binary, PREC_COMPARISON},
    [TOKEN_GREATER_EQUAL] = {NULL,     binary, PREC_COMPARISON},
    [TOKEN_IDENTIFIER]    = {NULL,     NULL,   PREC_NONE},
    [TOKEN_IF]            = {NULL,     NULL,   PREC_NONE},
    [TOKEN_LEFT_BRACE]    = {NULL,     NULL,   PREC_NONE},
    [TOKEN_LEFT_PAREN]    = {grouping, NULL,   PREC_NONE},
    [TOKEN_LESS]          = {NULL,     binary, PREC_COMPARISON},
    [TOKEN_LESS_EQUAL]    = {NULL,     binary, PREC_COMPARISON},
    [TOKEN_MINUS]         = {unary,    binary, PREC_TERM},
    [TOKEN_NIL]           = {literal,  NULL,   PREC_NONE},
    [TOKEN_NUMBER]        = {number,   NULL,   PREC_NONE},
    [TOKEN_OR]            = {NULL,     NULL,   PREC_NONE},
    [TOKEN_PLUS]          = {NULL,     binary, PREC_TERM},
    [TOKEN_PRINT]         = {NULL,     NULL,   PREC_NONE},
    [TOKEN_RETURN]        = {NULL,     NULL,   PREC_NONE},
    [TOKEN_RIGHT_BRACE]   = {NULL,     NULL,   PREC_NONE},
    [TOKEN_RIGHT_PAREN]   = {NULL,     NULL,   PREC_NONE},
    [TOKEN_SEMICOLON]     = {NULL,     NULL,   PREC_NONE},
    [TOKEN_SLASH]         = {NULL,     binary, PREC_FACTOR},
    [TOKEN_STAR]          = {NULL,     binary, PREC_FACTOR},
    [TOKEN_STRING]        = {NULL,     NULL,   PREC_NONE},
    [TOKEN_SUPER]         = {NULL,     NULL,   PREC_NONE},
    [TOKEN_THIS]          = {NULL,     NULL,   PREC_NONE},
    [TOKEN_TRUE]          = {literal,  NULL,   PREC_NONE},
    [TOKEN_VAR]           = {NULL,     NULL,   PREC_NONE},
    [TOKEN_WHILE]         = {NULL,     NULL,   PREC_NONE},
};

// clang-format on

/**
 * @brief Retrieves the parse rule for a given token type.
 *
 * @param type The type of the token.
 * @return ParseRule* Pointer to the corresponding parse rule.
 */
static ParseRule* getRule(TokenType type) { return &rules[type]; }

// Main Compiler Function

bool compile(const char* source, Chunk* chunk) {
  initScanner(source);
  compilingChunk = chunk;

  parser.hadError = false;
  parser.panicMode = false;

  advance();                                        // Start parsing.
  expression();                                     // Parse the top-level expression.
  consume(TOKEN_EOF, "Expect end of expression.");  // Ensure all tokens are consumed.
  endCompiler();                                    // Finish compilation.
  return !parser.hadError;                          // Return success if no errors occurred.
}
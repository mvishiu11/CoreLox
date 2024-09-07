#include "compiler.h"

#include <stdio.h>
#include <stdlib.h>

#include "common.h"

Parser parser;
Chunk* compilingChunk;

static Chunk* currentChunk() { return compilingChunk; }

// error reporting functionalities

static void errorAtCurrent(const char* message) { errorAt(&parser.current, message); }

static void error(const char* message) { errorAt(&parser.previous, message); }

static void errorAt(Token* token, const char* message) {
  if (parser.panicMode) return;
  parser.panicMode = true;
  fprintf(stderr, "[line %d] Error", token->line);

  if (token->type == TOKEN_EOF) {
    fprintf(stderr, " at end");
  } else if (token->type == TOKEN_ERROR) {
    // Nothing.
  } else {
    fprintf(stderr, " at '%.*s'", token->length, token->start);
  }

  fprintf(stderr, ": %s\n", message);
  parser.hadError = true;
}

// helper functions for moving through the token stream

static void advance() {
  parser.previous = parser.current;

  for (;;) {
    parser.current = scanToken();
    if (parser.current.type != TOKEN_ERROR) break;

    errorAtCurrent(parser.current.start);
  }
}

static void consume(TokenType type, const char* message) {
  if (parser.current.type == type) {
    advance();
    return;
  }

  errorAtCurrent(message);
}

// functions for emitting bytecode

static void emitByte(uint8_t byte) { writeChunk(currentChunk(), byte, parser.previous.line); }

static void emitBytes(uint8_t byte1, uint8_t byte2) {
  emitByte(byte1);
  emitByte(byte2);
}

static void emitConstant(Value value) {
  writeConstant(currentChunk(), value, parser.previous.line);
}

static void emitReturn() { emitByte(OP_RETURN); }

static void endCompiler() { emitReturn(); }

// functions for parsing expressions

static void parsePrecedence(Precedence precedence) {
  // What goes here?
}

static void expression() { parsePrecedence(PREC_ASSIGNMENT); }

static void grouping() {
  expression();
  consume(TOKEN_RIGHT_PAREN, "Expect ')' after expression.");
}

static void number() {
  double value = strtod(parser.previous.start, NULL);
  emitConstant(value);
}

static void unary() {
  TokenType operatorType = parser.previous.type;

  // Compile the operand.
  parsePrecedence(PREC_UNARY);

  // Emit the operator instruction.
  switch (operatorType) {
    case TOKEN_MINUS:
      emitByte(OP_NEGATE);
      break;
    default:
      return;  // Unreachable.
  }
}

// main compiler function

bool compile(const char* source, Chunk* chunk) {
  initScanner(source);
  compilingChunk = chunk;

  parser.hadError = false;
  parser.panicMode = false;

  advance();
  expression();
  consume(TOKEN_EOF, "Expect end of expression.");
  endCompiler();
  return !parser.hadError;
}
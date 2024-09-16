#include "compiler.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "common.h"
#include "memory.h"

#ifdef DEBUG_PRINT_CODE
#include "debug.h"
#endif

// Global parser state.
Parser parser;
Chunk* compilingChunk;
Compiler* current = NULL;

// Retrieves the current chunk being compiled.
static Chunk* currentChunk() { return &current->function->chunk; }

// Loop meta info encapsulation
static int* currentLoopDepth() { return &current->currentLoopDepth; }

static int* currentLoopStart() { return &current->currentLoopStart; }

static int* currentLoopEnd() { return &current->currentLoopEnd; }

static JumpList* currentBreakJumps() { return &current->breakJumps; }

//> Error Reporting Functions

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

static void errorAtCurrent(const char* message) { errorAt(&parser.current, message); }

static void error(const char* message) { errorAt(&parser.previous, message); }

//< Error Reporting Functionss
//> Helper Functions for Token Parsing

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

static bool tryConsume(TokenType type) {
  if (parser.current.type != type) return false;
  advance();
  return true;
}

static bool check(TokenType type) { return parser.current.type == type; }

static bool match(TokenType type) {
  if (!check(type)) return false;
  advance();
  return true;
}

//< Helper Functions for Token Parsing
//> Bytecode Emission Functions

static void emitByte(uint8_t byte) { writeChunk(currentChunk(), byte, parser.previous.line); }

static void emitBytes(uint8_t byte1, uint8_t byte2) {
  emitByte(byte1);
  emitByte(byte2);
}

static void emitConstant(Value value) {
  writeConstant(currentChunk(), value, parser.previous.line);
}

static void emitReturn() { emitByte(OP_NIL); emitByte(OP_RETURN); }

static void initCompiler(Compiler* compiler, FunctionType type) {
  compiler->enclosing = current;
  compiler->function = NULL;
  compiler->type = type;
  compiler->localCount = 0;
  compiler->scopeDepth = 0;
  compiler->currentLoopStart = -1;
  compiler->currentLoopEnd = -1;
  compiler->currentLoopDepth = 0;
  initJumpList(&compiler->breakJumps);
  compiler->localCapacity = UINT8_COUNT;
  compiler->locals = GROW_ARRAY(Local, NULL, 0, compiler->localCapacity);
  compiler->function = newFunction();
  current = compiler;
  if (type != TYPE_SCRIPT) {
    current->function->name = copyString(parser.previous.start,
                                         parser.previous.length);
  }

  Local* local = &current->locals[current->localCount++];
  local->depth = 0;
  local->name.start = "";
  local->name.length = 0;
}

static ObjFunction* endCompiler() {
  emitReturn();
  freeJumpList(currentBreakJumps());
  ObjFunction* function = current->function;

#ifdef DEBUG_PRINT_CODE
  if (!parser.hadError) {
    disassembleChunk(currentChunk(), function->name != NULL
        ? function->name->chars : "<script>");
  }
#endif

  current = current->enclosing;
  return function;
}

//< Bytecode Emission Functions
//> Scope Management Functions

static void beginScope() { current->scopeDepth++; }

static void endScope() {
  current->scopeDepth--;

  while (current->localCount > 0 &&
         current->locals[current->localCount - 1].depth > current->scopeDepth) {
    emitByte(OP_POP);
    current->localCount--;
  }
}

//< Scope Management Functions
//> Constants Table Functions

static uint8_t makeConstant(Value value) { return (uint8_t)addConstant(currentChunk(), value); }

//< Constants Table Functions

//> Parser meta utils (jumping, patching, synchronization etc.)

static void patchJump(int offset) {
  // -2 to adjust for the bytecode for the jump offset itself.
  int jump = currentChunk()->count - offset - 2;

  if (jump > UINT16_MAX) {
    error("Too much code to jump over.");
  }

  currentChunk()->code[offset] = (jump >> 8) & 0xff;
  currentChunk()->code[offset + 1] = jump & 0xff;
}

void patchJumps(JumpList* list, int depth, int target) {
  for (int i = list->count - 1; i >= 0; i--) {
    if (list->jumps[i].depth != depth) {
      break;
    }
    int jumpOffset = target - list->jumps[i].offset - 2;

    if (jumpOffset > UINT16_MAX) {
        error("Too much code to jump over.");
    }

    currentChunk()->code[list->jumps[i].offset] = (jumpOffset >> 8) & 0xff;
    currentChunk()->code[list->jumps[i].offset + 1] = jumpOffset & 0xff;
  }
}

static int emitJump(uint8_t instruction) {
  emitByte(instruction);
  emitByte(0xff);
  emitByte(0xff);
  return currentChunk()->count - 2;
}

static void emitLoop(int loopStart) {
  emitByte(OP_LOOP);

  int offset = currentChunk()->count - loopStart + 2;
  if (offset > UINT16_MAX) error("Loop body too large.");

  emitByte((offset >> 8) & 0xff);
  emitByte(offset & 0xff);
}

static void synchronize() {
  parser.panicMode = false;

  while (parser.current.type != TOKEN_EOF) {
    if (parser.previous.type == TOKEN_SEMICOLON) return;
    switch (parser.current.type) {
      case TOKEN_CLASS:
      case TOKEN_FUN:
      case TOKEN_VAR:
      case TOKEN_FOR:
      case TOKEN_IF:
      case TOKEN_WHILE:
      case TOKEN_PRINT:
      case TOKEN_RETURN:
        return;

      default:;  // Do nothing.
    }

    advance();
  }
}

//< Parser meta utils (jumping, patching, synchronization etc.)
//> Parsing Expression Functions

// Forward declarations

static void expression();
static void statement();
static void declaration();
static void parsePrecedence(Precedence precedence);
static ParseRule* getRule(TokenType type);

static void expression() { parsePrecedence(PREC_ASSIGNMENT); }

static void block() {
  while (!check(TOKEN_RIGHT_BRACE) && !check(TOKEN_EOF)) {
    declaration();
  }

  consume(TOKEN_RIGHT_BRACE, "Expect '}' after block.");
}

//< Parsing Expression Functions
//> Variable Parsing Functions

static uint8_t identifierConstant(Token* name) {
  return makeConstant(OBJ_VAL(copyString(name->start, name->length)));
}

static bool identifiersEqual(Token* a, Token* b) {
  if (a->length != b->length) return false;
  return memcmp(a->start, b->start, a->length) == 0;
}

static int resolveLocal(Compiler* compiler, Token* name) {
  for (int i = compiler->localCount - 1; i >= 0; i--) {
    Local* local = &compiler->locals[i];
    if (identifiersEqual(name, &local->name)) {
      if (local->depth == -1) {
        error("Can't read local variable in its own initializer.");
      }
      return i;
    }
  }

  return -1;
}

static void addLocal(Token name) {
  if (current->localCount == UINT8_COUNT) {
    error("Too many local variables in function.");
    return;
  }

  if (current->localCapacity < current->localCount + 1) {
    int oldCapacity = current->localCapacity;
    current->localCapacity = GROW_CAPACITY(oldCapacity);
    current->locals = GROW_ARRAY(Local, current->locals, oldCapacity, current->localCapacity);
  }
  Local* local = &current->locals[current->localCount++];
  local->name = name;
  local->depth = -1;
}

static void declareVariable() {
  if (current->scopeDepth == 0) return;

  Token* name = &parser.previous;

  for (int i = current->localCount - 1; i >= 0; i--) {
    Local* local = &current->locals[i];
    if (local->depth != -1 && local->depth < current->scopeDepth) {
      break;
    }

    if (identifiersEqual(name, &local->name)) {
      error("Already a variable with this name in this scope.");
    }
  }

  addLocal(*name);
}

static uint8_t parseVariable(const char* errorMessage) {
  consume(TOKEN_IDENTIFIER, errorMessage);

  declareVariable();
  if (current->scopeDepth > 0) return 0;

  return identifierConstant(&parser.previous);
}

static void markInitialized() {
  if (current->scopeDepth == 0) return;
  current->locals[current->localCount - 1].depth = current->scopeDepth;
}

static void defineVariable(uint8_t global) {
  if (current->scopeDepth > 0) {
    markInitialized();
    return;
  }

  emitBytes(OP_DEFINE_GLOBAL, global);
}

//< Variable Parsing Functions
//> Function Parsing Functions

static void function(FunctionType type) {
  Compiler compiler;
  initCompiler(&compiler, type);
  beginScope(); 

  consume(TOKEN_LEFT_PAREN, "Expect '(' after function name.");

  if (!check(TOKEN_RIGHT_PAREN)) {
    do {
      current->function->arity++;
      if (current->function->arity > 255) {
        errorAtCurrent("Can't have more than 255 parameters.");
      }
      uint8_t constant = parseVariable("Expect parameter name.");
      defineVariable(constant);
    } while (match(TOKEN_COMMA));
  }

  consume(TOKEN_RIGHT_PAREN, "Expect ')' after parameters.");
  consume(TOKEN_LEFT_BRACE, "Expect '{' before function body.");
  block();

  ObjFunction* function = endCompiler();
  emitBytes(OP_CLOSURE, makeConstant(OBJ_VAL(function)));
}

static void funDeclaration() {
  uint8_t global = parseVariable("Expect function name.");
  markInitialized();
  function(TYPE_FUNCTION);
  defineVariable(global);
}

static uint8_t argumentList() {
  uint8_t argCount = 0;
  if (!check(TOKEN_RIGHT_PAREN)) {
    do {
      expression();
      argCount++;
    } while (match(TOKEN_COMMA));
  }
  consume(TOKEN_RIGHT_PAREN, "Expect ')' after arguments.");
  return argCount;
}

//< Function Parsing Functionss
//> Control Flow Parsing Functions

static void and_(bool canAssign __attribute__((unused))) {
  int endJump = emitJump(OP_JUMP_IF_FALSE);

  emitByte(OP_POP);
  parsePrecedence(PREC_AND);

  patchJump(endJump);
}

static void or_(bool canAssign __attribute__((unused))) {
  int endJump = emitJump(OP_JUMP_IF_TRUE);

  emitByte(OP_POP);

  parsePrecedence(PREC_OR);
  patchJump(endJump);
}

static void elifStatement();

static void elifStatement() {
  if (!tryConsume(TOKEN_LEFT_PAREN)) {
    expression();
    consume(TOKEN_THEN, "Expect 'then' after expression without parantheses.");
  } else {
    expression();
    consume(TOKEN_RIGHT_PAREN, "Expect ')' after condition.");
  }

  int thenJump = emitJump(OP_JUMP_IF_FALSE);
  emitByte(OP_POP);
  statement();

  int elseJump = emitJump(OP_JUMP);

  patchJump(thenJump);
  emitByte(OP_POP);

  if (match(TOKEN_ELIF)) {
    elifStatement();
  } else if (match(TOKEN_ELSE)) {
    statement();
  }

  patchJump(elseJump);
}

static void ifStatement() {
  if (!tryConsume(TOKEN_LEFT_PAREN)) {
    expression();
    consume(TOKEN_THEN, "Expect 'then' after expression without parantheses.");
  } else {
    expression();
    consume(TOKEN_RIGHT_PAREN, "Expect ')' after condition.");
  }

  int thenJump = emitJump(OP_JUMP_IF_FALSE);
  emitByte(OP_POP);
  statement();

  int elseJump = emitJump(OP_JUMP);

  patchJump(thenJump);
  emitByte(OP_POP);

  if (match(TOKEN_ELIF)) {
    elifStatement();
  } else if (match(TOKEN_ELSE)) {
    statement();
  }

  patchJump(elseJump);
}

//< Control Flow Parsing Functions
//> Parsing function for statements and declarations

static void varDeclaration() {
  uint8_t global = parseVariable("Expect variable name.");

  if (match(TOKEN_EQUAL)) {
    expression();
  } else {
    emitByte(OP_NIL);
  }
  consume(TOKEN_SEMICOLON, "Expect ';' after variable declaration.");

  defineVariable(global);
}

static void expressionStatement() {
  expression();
  consume(TOKEN_SEMICOLON, "Expect ';' after expression.");
  emitByte(OP_POP);
}

static void printStatement() {
  expression();
  consume(TOKEN_SEMICOLON, "Expect ';' after value.");
  emitByte(OP_PRINT);
}

static void whileStatement() {
  int surroundingLoopStart = *currentLoopStart();
  int surroundingLoopEnd = *currentLoopEnd();
  *currentLoopDepth() += 1;

  int loopStart = currentChunk()->count;
  *currentLoopStart() = loopStart;
  if (!tryConsume(TOKEN_LEFT_PAREN)) {
    expression();
    consume(TOKEN_THEN, "Expect 'then' after expression without parantheses.");
  } else {
    expression();
    consume(TOKEN_RIGHT_PAREN, "Expect ')' after condition.");
  }

  int exitJump = emitJump(OP_JUMP_IF_FALSE);
  emitByte(OP_POP);
  statement();
  emitLoop(loopStart);

  patchJump(exitJump);
  emitByte(OP_POP);
  *currentLoopEnd() = currentChunk()->count;

  patchJumps(currentBreakJumps(), *currentLoopDepth(), *currentLoopEnd());
  *currentLoopDepth() -= 1;
  *currentLoopStart() = surroundingLoopStart;
  *currentLoopEnd() = surroundingLoopEnd;
}

static void forStatement() {
  int surroundingLoopStart = *currentLoopStart();
  int surroundingLoopEnd = *currentLoopEnd();
  *currentLoopDepth() += 1;

  beginScope();
  consume(TOKEN_LEFT_PAREN, "Expect '(' after 'for'.");

  // Initializer.
  if (match(TOKEN_SEMICOLON)) {
    // No initializer.
  } else if (match(TOKEN_VAR)) {
    varDeclaration();
  } else {
    expressionStatement();
  }

  // Condition.
  int loopStart = currentChunk()->count;
  *currentLoopStart() = loopStart;
  int exitJump = -1;
  if (!match(TOKEN_SEMICOLON)) {
    expression();
    consume(TOKEN_SEMICOLON, "Expect ';' after loop condition.");

    // Jump out of the loop if the condition is false.
    exitJump = emitJump(OP_JUMP_IF_FALSE);
    emitByte(OP_POP); // Condition.
  }

  // Increment.
  if (!match(TOKEN_RIGHT_PAREN)) {
    int bodyJump = emitJump(OP_JUMP);
    int incrementStart = currentChunk()->count;
    expression();
    emitByte(OP_POP);
    consume(TOKEN_RIGHT_PAREN, "Expect ')' after for clauses.");

    emitLoop(loopStart);
    loopStart = incrementStart;
    *currentLoopStart() = incrementStart;
    patchJump(bodyJump);
  }

  statement();
  emitLoop(loopStart);

  if (exitJump != -1) {
    patchJump(exitJump);
    emitByte(OP_POP); // Condition.
  }

  *currentLoopEnd() = currentChunk()->count;

  patchJumps(currentBreakJumps(), *currentLoopDepth(), *currentLoopEnd());
  *currentLoopDepth() -= 1;
  *currentLoopStart() = surroundingLoopStart;
  *currentLoopEnd() = surroundingLoopEnd;
  endScope();
}

bool fallthroughMode = false;
int fallJump = -1;

// switchCase → "case" expression ":" statement* ;
static int switchCase() {
    consume(TOKEN_CASE, "Expected 'case' here.");
    emitByte(OP_DUP);
    expression();
    consume(TOKEN_COLON, "Expect ':' after case expression.");

    emitByte(OP_EQUAL);
    int caseFalseJump = emitJump(OP_JUMP_IF_FALSE);
    emitByte(OP_POP);

    if (fallthroughMode) {
      patchJump(fallJump);
      fallthroughMode = false;
    }

    while (!check(TOKEN_CASE) && !check(TOKEN_DEFAULT) && !check(TOKEN_RIGHT_BRACE) && !check(TOKEN_FALLTHROUGH)) {
        statement();
    }

    if (!check(TOKEN_FALLTHROUGH)) {
      int caseEndJump = emitJump(OP_JUMP);
      patchJump(caseFalseJump);
      emitByte(OP_POP);
      fallthroughMode = false;
      return caseEndJump;
    } else {
      consume(TOKEN_FALLTHROUGH, "Expected 'fallthrough' here.");  // If this throws, something is super wrong.
      fallthroughMode = true;
      fallJump = emitJump(OP_JUMP);
      patchJump(caseFalseJump);
      emitByte(OP_POP);
      return -1;
    }
}

// defaultCase → "default" ":" statement* ;
static void defaultCase() {
    consume(TOKEN_COLON, "Expect ':' after 'default'.");

    if (fallthroughMode) {
      patchJump(fallJump);
      fallthroughMode = false;
    }

    while (!check(TOKEN_RIGHT_BRACE)) {
        statement();
    }
}

// switchStmt → "switch" "(" expression ")" "{" switchCase* defaultCase? "}" ;
static void switchStatement() {
    consume(TOKEN_LEFT_PAREN, "Expect '(' after 'switch'.");
    expression();
    consume(TOKEN_RIGHT_PAREN, "Expect ')' after switch value.");

    consume(TOKEN_LEFT_BRACE, "Expect '{' before switch cases.");

    JumpList caseJumps;
    initJumpList(&caseJumps);

    while (check(TOKEN_CASE)) {
        int caseEndJump = switchCase();
        if(caseEndJump != -1) addJump(&caseJumps, *currentLoopDepth(), caseEndJump);
    }

    if (match(TOKEN_DEFAULT)) {
        defaultCase();
    }

    patchJumps(&caseJumps, *currentLoopDepth(), currentChunk()->count);
    freeJumpList(&caseJumps);

    emitByte(OP_POP);  // Pop the switch value

    consume(TOKEN_RIGHT_BRACE, "Expect '}' after switch cases.");
}

static void returnStatement() {
  if (current->type == TYPE_SCRIPT) {
    error("Can't return from top-level code.");
  }
  
  if (match(TOKEN_SEMICOLON)) {
    emitReturn();
  } else {
    expression();
    consume(TOKEN_SEMICOLON, "Expect ';' after return value.");
    emitByte(OP_RETURN);
  }
}

static void breakStatement() {
  if (*currentLoopStart() == -1) {
    error("Cannot use 'break' outside of a loop.");
    return;
  }

  consume(TOKEN_SEMICOLON, "Expect ';' after 'break'.");
  int jump = emitJump(OP_JUMP);
  emitByte(OP_POP);
  addJump(currentBreakJumps(), *currentLoopDepth(), jump);
}

static void continueStatement() {
  if (*currentLoopStart() == -1) {
    error("Cannot use 'continue' outside of a loop.");
    return;
  }

  consume(TOKEN_SEMICOLON, "Expect ';' after 'break'.");
  emitLoop(*currentLoopStart());
}

static void declaration() {
  if (match(TOKEN_FUN)) {
    funDeclaration();
  } else if (match(TOKEN_VAR)) {
    varDeclaration();
  } else {
    statement();
  }

  if (parser.panicMode) synchronize();
}

static void statement() {
  if (match(TOKEN_PRINT)) {
    printStatement();
  } else if (match(TOKEN_IF)) {
    ifStatement();
  } else if (match(TOKEN_FOR)) {
    forStatement();
  } else if (match(TOKEN_WHILE)) {
    whileStatement();
  } else if (match(TOKEN_SWITCH)) {
    switchStatement();
  } else if (match(TOKEN_RETURN)) {
    returnStatement();
  } else if (match(TOKEN_BREAK)) {
    breakStatement();
  } else if (match(TOKEN_CONTINUE)) {
    continueStatement();
  } else if (match(TOKEN_LEFT_BRACE)) {
    beginScope();
    block();
    endScope();
  } else {
    expressionStatement();
  }
}

//< Parsing function for statements and declarations
//> Parsing primitives

static void binary(bool canAssign __attribute__((unused))) {
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
    case TOKEN_PERCENT:
      emitByte(OP_MODULO);
      break;
    default:
      return;  // Unreachable.
  }
}

static void call(bool canAssign __attribute__((unused))) {
  uint8_t argCount = argumentList();
  emitBytes(OP_CALL, argCount);
}

static void ternary(bool canAssign __attribute__((unused))) {
  int thenJump = emitJump(OP_JUMP_IF_FALSE);
  emitByte(OP_POP);

  parsePrecedence(PREC_TERNARY);

  consume(TOKEN_COLON, "Expect ':' after true branch of ternary operator.");

  int elseJump = emitJump(OP_JUMP);

  patchJump(thenJump);
  emitByte(OP_POP);

  parsePrecedence(PREC_ASSIGNMENT);

  patchJump(elseJump);
}


static void literal(bool canAssign __attribute__((unused))) {
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

static void grouping(bool canAssign __attribute__((unused))) {
  expression();
  consume(TOKEN_RIGHT_PAREN, "Expect ')' after expression.");
}

static void number(bool canAssign __attribute__((unused))) {
  double value = strtod(parser.previous.start, NULL);
  emitConstant(NUMBER_VAL(value));
}

static void string(bool canAssign __attribute__((unused))) {
  emitConstant(OBJ_VAL(copyString(parser.previous.start + 1, parser.previous.length - 2)));
}

static void namedVariable(Token name, bool canAssign) {
  uint8_t getOp, setOp;
  int arg = resolveLocal(current, &name);
  if (arg != -1) {
    getOp = OP_GET_LOCAL;
    setOp = OP_SET_LOCAL;
  } else {
    arg = identifierConstant(&name);
    getOp = OP_GET_GLOBAL;
    setOp = OP_SET_GLOBAL;
  }

  if (canAssign && match(TOKEN_EQUAL)) {
    expression();
    emitBytes(setOp, (uint8_t)arg);
  } else {
    emitBytes(getOp, (uint8_t)arg);
  }
}

static void variable(bool canAssign) { namedVariable(parser.previous, canAssign); }

static void unary(bool canAssign __attribute__((unused))) {
  TokenType operatorType = parser.previous.type;
  parsePrecedence(PREC_UNARY);

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

//< Parsing primitives
//> Parsing helpers

/**
 * @brief Parses an expression based on operator precedence.
 *
 * This function parses expressions using a precedence climbing
 * algorithm, handling operators based on their precedence levels.
 *
 * @param precedence The precedence of the current operator.
 */
static void parsePrecedence(Precedence precedence) {
  advance();
  ParseFn prefixRule = getRule(parser.previous.type)->prefix;
  if (prefixRule == NULL) {
    error("Expect expression.");
    return;
  }

  bool canAssign = precedence <= PREC_ASSIGNMENT;
  prefixRule(canAssign);

  while (precedence <= getRule(parser.current.type)->precedence) {
    advance();
    ParseFn infixRule = getRule(parser.previous.type)->infix;
    infixRule(canAssign);
  }

  if (canAssign && match(TOKEN_EQUAL)) {
    error("Invalid assignment target.");
  }
}

//< Parsing helpers
//> Parse Rules and Operator Precedence Table

// clang-format off

ParseRule rules[] = {
    [TOKEN_AND]           = {NULL,     and_,    PREC_AND},
    [TOKEN_BANG]          = {unary,    NULL,    PREC_NONE},
    [TOKEN_BANG_EQUAL]    = {NULL,     binary,  PREC_EQUALITY},
    [TOKEN_CLASS]         = {NULL,     NULL,    PREC_NONE},
    [TOKEN_COMMA]         = {NULL,     NULL,    PREC_NONE},
    [TOKEN_DOT]           = {NULL,     NULL,    PREC_NONE},
    [TOKEN_ELSE]          = {NULL,     NULL,    PREC_NONE},
    [TOKEN_EQUAL]         = {NULL,     NULL,    PREC_NONE},
    [TOKEN_EQUAL_EQUAL]   = {NULL,     binary,  PREC_EQUALITY},
    [TOKEN_EOF]           = {NULL,     NULL,    PREC_NONE},
    [TOKEN_ERROR]         = {NULL,     NULL,    PREC_NONE},
    [TOKEN_FALSE]         = {literal,  NULL,    PREC_NONE},
    [TOKEN_FOR]           = {NULL,     NULL,    PREC_NONE},
    [TOKEN_FUN]           = {NULL,     NULL,    PREC_NONE},
    [TOKEN_GREATER]       = {NULL,     binary,  PREC_COMPARISON},
    [TOKEN_GREATER_EQUAL] = {NULL,     binary,  PREC_COMPARISON},
    [TOKEN_IDENTIFIER]    = {variable, NULL,    PREC_NONE},
    [TOKEN_IF]            = {NULL,     NULL,    PREC_NONE},
    [TOKEN_LEFT_BRACE]    = {NULL,     NULL,    PREC_NONE},
    [TOKEN_LEFT_PAREN]    = {grouping, call,   PREC_CALL},
    [TOKEN_LESS]          = {NULL,     binary,  PREC_COMPARISON},
    [TOKEN_LESS_EQUAL]    = {NULL,     binary,  PREC_COMPARISON},
    [TOKEN_MINUS]         = {unary,    binary,  PREC_TERM},
    [TOKEN_NIL]           = {literal,  NULL,    PREC_NONE},
    [TOKEN_NUMBER]        = {number,   NULL,    PREC_NONE},
    [TOKEN_OR]            = {NULL,     or_,     PREC_OR},
    [TOKEN_PERCENT]       = {NULL,     binary,  PREC_FACTOR},
    [TOKEN_PLUS]          = {NULL,     binary,  PREC_TERM},
    [TOKEN_PRINT]         = {NULL,     NULL,    PREC_NONE},
    [TOKEN_QUESTION]      = {NULL,     ternary, PREC_TERNARY},
    [TOKEN_RETURN]        = {NULL,     NULL,    PREC_NONE},
    [TOKEN_RIGHT_BRACE]   = {NULL,     NULL,    PREC_NONE},
    [TOKEN_RIGHT_PAREN]   = {NULL,     NULL,    PREC_NONE},
    [TOKEN_SEMICOLON]     = {NULL,     NULL,    PREC_NONE},
    [TOKEN_SLASH]         = {NULL,     binary,  PREC_FACTOR},
    [TOKEN_STAR]          = {NULL,     binary,  PREC_FACTOR},
    [TOKEN_STRING]        = {string,   NULL,    PREC_NONE},
    [TOKEN_SUPER]         = {NULL,     NULL,    PREC_NONE},
    [TOKEN_THIS]          = {NULL,     NULL,    PREC_NONE},
    [TOKEN_TRUE]          = {literal,  NULL,    PREC_NONE},
    [TOKEN_VAR]           = {NULL,     NULL,    PREC_NONE},
    [TOKEN_WHILE]         = {NULL,     NULL,    PREC_NONE},
};

// clang-format on

static ParseRule* getRule(TokenType type) { return &rules[type]; }

// Main Compiler Function

ObjFunction* compile(const char* source) {
  initScanner(source);
  Compiler compiler;
  initCompiler(&compiler, TYPE_SCRIPT);

  parser.hadError = false;
  parser.panicMode = false;

  advance();

  while (!match(TOKEN_EOF)) {
    declaration();
  }

  ObjFunction* function = endCompiler();
  return parser.hadError ? NULL : function;
}
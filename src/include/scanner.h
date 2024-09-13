#ifndef corelox_scanner_h
#define corelox_scanner_h

/**
 * @file scanner.h
 * @brief Lexical scanner for tokenizing source code.
 *
 * This header defines the types and functions for tokenizing the
 * source code into tokens. The scanner is responsible for reading
 * the raw source code and breaking it into a series of tokens that
 * the parser can then process.
 */

/**
 * @brief Enumeration of all possible token types.
 *
 * This enum defines the different types of tokens that can be
 * recognized by the scanner. It includes single-character tokens
 * (like `+` and `-`), multi-character tokens (like `<=`), literals,
 * keywords, and special tokens like `TOKEN_ERROR` and `TOKEN_EOF`.
 */
typedef enum {
  // Single-character tokens.
  TOKEN_LEFT_PAREN,   ///< '('
  TOKEN_RIGHT_PAREN,  ///< ')'
  TOKEN_LEFT_BRACE,   ///< '{'
  TOKEN_RIGHT_BRACE,  ///< '}'
  TOKEN_COMMA,        ///< ','
  TOKEN_DOT,          ///< '.'
  TOKEN_MINUS,        ///< '-'
  TOKEN_PLUS,         ///< '+'
  TOKEN_SEMICOLON,    ///< ';'
  TOKEN_SLASH,        ///< '/'
  TOKEN_STAR,         ///< '*'

  // One or two character tokens.
  TOKEN_BANG,           ///< '!'
  TOKEN_BANG_EQUAL,     ///< '!='
  TOKEN_EQUAL,          ///< '='
  TOKEN_EQUAL_EQUAL,    ///< '=='
  TOKEN_GREATER,        ///< '>'
  TOKEN_GREATER_EQUAL,  ///< '>='
  TOKEN_LESS,           ///< '<'
  TOKEN_LESS_EQUAL,     ///< '<='

  // Literals.
  TOKEN_IDENTIFIER,  ///< Identifiers (variable names, function names, etc.).
  TOKEN_STRING,      ///< String literals.
  TOKEN_NUMBER,      ///< Number literals (integers, floats).

  // Keywords.
  TOKEN_AND,     ///< `and` keyword.
  TOKEN_CLASS,   ///< `class` keyword.
  TOKEN_ELSE,    ///< `else` keyword.
  TOKEN_ELIF,    ///< `elif` keyword.
  TOKEN_FALSE,   ///< `false` keyword.
  TOKEN_FOR,     ///< `for` keyword.
  TOKEN_FUN,     ///< `fun` keyword (for function declarations).
  TOKEN_IF,      ///< `if` keyword.
  TOKEN_NIL,     ///< `nil` keyword (null value).
  TOKEN_OR,      ///< `or` keyword.
  TOKEN_PRINT,   ///< `print` keyword.
  TOKEN_RETURN,  ///< `return` keyword.
  TOKEN_SUPER,   ///< `super` keyword (for superclass method calls).
  TOKEN_THEN,    ///< `then` keyword. Used in the `if` statement.
  TOKEN_THIS,    ///< `this` keyword (current class instance).
  TOKEN_TRUE,    ///< `true` keyword.
  TOKEN_VAR,     ///< `var` keyword (for variable declarations).
  TOKEN_WHILE,   ///< `while` keyword (for loops).

  // Special tokens.
  TOKEN_ERROR,  ///< Represents a scanning error.
  TOKEN_EOF     ///< End of file token.
} TokenType;

/**
 * @brief Represents the state of the scanner during tokenization.
 *
 * The `Scanner` struct keeps track of the current position in the source
 * code during scanning. It holds pointers to the start of the current
 * token being scanned, the current position in the source, and the
 * current line number.
 */
typedef struct {
  const char* start;    ///< Pointer to the start of the current token.
  const char* current;  ///< Pointer to the current character being scanned.
  int line;             ///< The current line number in the source code.
} Scanner;

/**
 * @brief Represents a single token.
 *
 * The `Token` struct holds the type of the token, a pointer to the start
 * of the token in the source code, the length of the token, and the line
 * number where the token appears. It is used to pass individual tokens
 * from the scanner to the parser.
 */
typedef struct {
  TokenType type;     ///< The type of the token (e.g., `TOKEN_PLUS`, `TOKEN_IF`).
  const char* start;  ///< Pointer to the start of the token in the source code.
  int length;         ///< The length of the token in characters.
  int line;           ///< The line number where the token appears.
} Token;

/**
 * @brief Initializes the scanner with the source code.
 *
 * This function sets up the scanner to start reading from the given
 * source code string. It initializes the scanner's internal state,
 * setting the current position to the beginning of the source.
 *
 * @param source Pointer to the source code string to be scanned.
 */
void initScanner(const char* source);

/**
 * @brief Scans the next token from the source code.
 *
 * This function reads the next token from the source code, updates the
 * scanner's state, and returns the token. It handles all types of tokens,
 * including keywords, literals, and special characters. If an error occurs
 * during scanning, a `TOKEN_ERROR` token is returned.
 *
 * @return The next `Token` in the source code.
 */
Token scanToken();

#endif

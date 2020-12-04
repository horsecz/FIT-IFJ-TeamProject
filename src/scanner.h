/**
 * Project: Implementace překladače imperativního jazyka IFJ20
 *
 * @name scanner.h
 * @brief Scanner header file containing various definitions for scanner to use.
 *
 * @authors
 *      Hais Lukas (xhaisl00)
 */

#ifndef IFJ_PROJ_SCANNER_H
#define IFJ_PROJ_SCANNER_H

// include for int_64t and bool support
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
// include files for working with dynamic string
#include "str.h"

char hexaHold[2];

typedef enum
{
  STATE_START,

  /** THOSE 4 STATES HAVE UNARY EXTENSION ALREADY **/
  STATE_PLUS, // could be plus operator, positive number or plus assign
  STATE_MINUS, // could be minus operator, negative number or minus assign
  STATE_MULTIPLY, // could be multiplication operator or multiply assign
  STATE_DIVIDE, // could result in [multi] line comment, division operator or divide assign

  STATE_LINE_COMMENT,
  STATE_MULTI_LINE_COMMENT_START,
  STATE_MULTI_LINE_COMMENT_END,

  STATE_IDENTIFIER, // if it starts with '_'
  STATE_IDENTIFIER_OR_KEYWORD,

  STATE_DIGIT,
  STATE_DIGIT_WITH_EXP,
  STATE_DIGIT_WITH_EXP_AND_OP,
  STATE_DIGIT_WITH_EXP_AND_OP_NUM,
  STATE_FLOAT,
  STATE_FLOAT_WITH_EXP,
  STATE_FLOAT_WITH_EXP_AND_OP,
  STATE_FLOAT_WITH_EXP_AND_OP_NUM,

  STATE_STRING,
  STATE_STRING_SKIP, // used when '\' is read in string
  STATE_STRING_HEXA_START,
  STATE_STRING_HEXA_END,

  STATE_DECLARATIVE_ASSIGN,
  STATE_NOT_EQUALS,

  STATE_LESSER_OR_EQUAL,
  STATE_GREATER_OR_EQUAL,

  // BOOLEAN EXTENSION
  STATE_AND,
  STATE_OR,
} State;

/**
 * @enum Predefined keywords for @union TokenAttribute.
 */
typedef enum
{
  // PACKAGES
  KEYWORD_PACKAGE, // 0
  KEYWORD_IMPORT, // 1

  // FUNCTIONS AND RETURN
  KEYWORD_FUNC, // 2
  KEYWORD_RETURN, // 3
  KEYWORD_PRINT, // 4

  // VARIABLES
  KEYWORD_VAR, // 5
  KEYWORD_CONST, // 6

  // FLOW CONTROL
  KEYWORD_IF, // 7
  KEYWORD_ELSE, // 8
  KEYWORD_SWITCH, // 9
  KEYWORD_CASE, // 10
  KEYWORD_DEFAULT, // 11
  KEYWORD_FOR, // 12

  // DATA TYPES
  KEYWORD_STRING, // 13
  KEYWORD_INT, // 14
  KEYWORD_FLOAT64, // 15

  // BOOLEAN EXTENSION
  KEYWORD_BOOL, // 16

  // EMPTY, FOR INIT
  KEYWORD_EMPTY // 17
} Keyword;

/**
 * @enum Predefined @struct Token types.
 */
typedef enum
{
  // BASIC
  TYPE_EOF, // 0
  TYPE_EOL,
  TYPE_EMPTY, // used when scanner encounters comment
  TYPE_IDENTIFIER,
  TYPE_KEYWORD, // 4

  // DATA TYPES
  TYPE_INT,
  TYPE_STRING,
  TYPE_FLOAT64,
  TYPE_BOOL, // 8

  // ARITHMETIC OPERATORS
  TYPE_PLUS,
  TYPE_MINUS,
  TYPE_MULTIPLY,
  TYPE_DIVIDE, // 12

  // RELATIONAL OPERATORS
  TYPE_EQUALS,
  TYPE_NOT_EQUALS,
  TYPE_GREATER,
  TYPE_LESSER,
  TYPE_GREATER_OR_EQUAL,
  TYPE_LESSER_OR_EQUAL, // 18

  // LOGICAL OPERATORS // BOOLEAN OPERATION
  TYPE_AND,
  TYPE_OR,
  TYPE_NOT, // 21

  // ASSIGNMENT OPERATORS
  TYPE_ASSIGN,
  TYPE_DECLARATIVE_ASSIGN,
  TYPE_PLUS_ASSIGN,
  TYPE_MINUS_ASSIGN,
  TYPE_MULTIPLY_ASSIGN,
  TYPE_DIVIDE_ASSIGN,
  TYPE_MODULO_ASSIGN, // 28

  // BRACKET TYPES
  TYPE_LEFT_BRACKET,
  TYPE_RIGHT_BRACKET,
  TYPE_LEFT_CURLY_BRACKET,
  TYPE_RIGHT_CURLY_BRACKET, // 32
<<<<<<< HEAD:scanner.h

  // Precedent Tokens
  TOKEN_PREC_ID,
  TOKEN_PREC_INTEGER,
  TOKEN_PREC_FLOAT,
  TOKEN_PREC_STRING,
  TOKEN_PREC_BOOL,
  TOKEN_PREC_CLOSE,
  TOKEN_PREC_OPEN,

=======
>>>>>>> parser-dev:src/scanner.h

  // DELIMITERS
  TYPE_COMMA,
  TYPE_SEMICOLON, // 34
} TokenType;

/**
 * @union Various attributes that could be saved for @struct Token. Only one
 *          is active at a time.
 */
typedef union
{
  string string;
  int64_t integer;
  double float64;
  bool boolean;

  Keyword keyword;
} TokenAttribute;

/**
 * @struct Represents lexical token which holds @enum TokenType as type of
 *          token and assigned @union TokenAttribute as attribute of token.
 */
typedef struct
{
  TokenType type;
  TokenAttribute attribute;
} Token;

/**
 * @brief Checks whether passed string equals to any reserved keyword and if so
 *          it returns corresponding code to keyword
 *
 * @param str - string to compare
 * @return 0 if no match found
 *         1-17 depending on match
 */
int isReservedKeyword (string *str);

/**
 * @brief Sets type of @c token to corresponding keyword type by @c keywordType
 *
 * @param token
 * @param keywordType
 */
void setTokenKeyword (Token *token, int keywordType);

/**
 * @brief Checks whether character @c c escaped by '\' is valid by GO
 *
 * @param c
 * @return 1 - valid
 *         0 - invalid
 */
int isValidEscapeCharacter (char c);

/**
 * @brief Gets escape sequence by @c c
 *
 * @param c
 * @return character containing escape sequence
 */
char getEscapeSequence (char c);

/**
 * @brief Reads char after char from stdin
 *
 * @param token
 * @return 0 if successfully returned token,
 *         1 if lexical error,
 *         2 if token equals NULL,
 *         99 if internal problem
 */
int getToken (Token *token);
#endif //IFJ_PROJ_SCANNER_H

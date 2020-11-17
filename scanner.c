/**
 * Project: Implementace překladače imperativního jazyka IFJ20
 *
 * @file scanner.c
 * @brief Definition of scanner's functions
 *
 * @authors
 *      Hais Lukas (xhaisl00)
 */

#include <ctype.h>
#include <stdlib.h>
#include "scanner.h"

#define DEBUG 1
#define SCANNER_SUCC 0
#define SCANNER_ERR 1
#define SCANNER_MISSING_TOKEN 2
#define SCANNER_INTERNAL 99

int isReservedKeyword (string *str)
{
  // maybe this is not the best approach (discussion necessary)
  if (strCmpConstStr(str, "package") == 0)
    return 1;
  else if (strCmpConstStr(str, "import") == 0)
    return 2;
  else if (strCmpConstStr(str, "func") == 0)
    return 3;
  else if (strCmpConstStr(str, "return") == 0)
    return 4;
  else if (strCmpConstStr(str, "print") == 0)
    return 5;
  else if (strCmpConstStr(str, "var") == 0)
    return 6;
  else if (strCmpConstStr(str, "const") == 0)
    return 7;
  else if (strCmpConstStr(str, "if") == 0)
    return 8;
  else if (strCmpConstStr(str, "else") == 0)
    return 9;
  else if (strCmpConstStr(str, "switch") == 0)
    return 10;
  else if (strCmpConstStr(str, "case") == 0)
    return 11;
  else if (strCmpConstStr(str, "default") == 0)
    return 12;
  else if (strCmpConstStr(str, "for") == 0)
    return 13;
  else if (strCmpConstStr(str, "string") == 0)
    return 14;
  else if (strCmpConstStr(str, "int") == 0)
    return 15;
  else if (strCmpConstStr(str, "float64") == 0)
    return 16;
  else if (strCmpConstStr(str, "bool") == 0)
    return 17;
  else if (strCmpConstStr(str, "true") == 0)
    return 18;
  else if (strCmpConstStr(str, "false") == 0)
    return 19;
  else return 0;
}

void setTokenKeyword (Token *token, int keywordType)
{
  token->type = TYPE_KEYWORD;
  switch (keywordType)
  {
    case 1:
      token->attribute.keyword = KEYWORD_PACKAGE;
      break;
    case 2:
      token->attribute.keyword = KEYWORD_IMPORT;
      break;
    case 3:
      token->attribute.keyword = KEYWORD_FUNC;
      break;
    case 4:
      token->attribute.keyword = KEYWORD_RETURN;
      break;
    case 5:
      token->attribute.keyword = KEYWORD_PRINT;
      break;
    case 6:
      token->attribute.keyword = KEYWORD_VAR;
      break;
    case 7:
      token->attribute.keyword = KEYWORD_CONST;
      break;
    case 8:
      token->attribute.keyword = KEYWORD_IF;
      break;
    case 9:
      token->attribute.keyword = KEYWORD_ELSE;
      break;
    case 10:
      token->attribute.keyword = KEYWORD_SWITCH;
      break;
    case 11:
      token->attribute.keyword = KEYWORD_CASE;
      break;
    case 12:
      token->attribute.keyword = KEYWORD_DEFAULT;
      break;
    case 13:
      token->attribute.keyword = KEYWORD_FOR;
      break;
    case 14:
      token->attribute.keyword = KEYWORD_STRING;
      break;
    case 15:
      token->attribute.keyword = KEYWORD_INT;
      break;
    case 16:
      token->attribute.keyword = KEYWORD_FLOAT64;
      break;
    case 17:
      token->attribute.keyword = KEYWORD_BOOL;
      break;
    case 18:
      token->attribute.keyword = KEYWORD_TRUE;
      break;
    case 19:
      token->attribute.keyword = KEYWORD_FALSE;
  }
}

int isValidEscapeCharacter (char c)
{
  switch (c)
  {
    case 'a':
    case 'b':
    case 'f':
    case 'n':
    case 'r':
    case 't':
    case 'v':
    case '\\':
    case '"':
      return 1;
    default:
      return 0;
  }
}

char getEscapeSequence (char c)
{
  switch (c)
  {
    case 'a':
      return '\a';
    case 'b':
      return '\b';
    case 'f':
      return '\f';
    case 'n':
      return '\n';
    case 'r':
      return '\r';
    case 't':
      return '\t';
    case 'v':
      return '\v';
    case '\\':
      return '\\';
    case '"':
      return '\"';
    default:
      return '\0';
  }
}

int getToken (Token *token)
{
  if (token == NULL)
  {
    return SCANNER_MISSING_TOKEN;
  }

  // char read from stdin
  char c;
  // initialization of state
  State state = STATE_START;

  while (1)
  {
    // read another character
    c = getc(stdin);

    // enter deterministic automaton
    switch (state)
    {
      // TODO: Retezcovy literal specifikace escape sequence
      case STATE_START:
        if (c == EOF)
        {
          if (DEBUG) printf("[EOF]\n");
          token->type = TYPE_EOF;
          return SCANNER_SUCC;
        }
        else if (c == '+')
        {
          state = STATE_PLUS;
        }
        else if (c == '-')
        {
          state = STATE_MINUS;
        }
        else if (c == '*')
        {
          state = STATE_MULTIPLY;
        }
        else if (c == '/')
        {
          state = STATE_DIVIDE;
        }
        else if (c == '_' || isalpha(c))
        {
          // decide if identifier only by leading underscore, ID/KEY otherwise
          if (c == '_')
          {
            state = STATE_IDENTIFIER;
          }
          else
          {
            state = STATE_IDENTIFIER_OR_KEYWORD;
          }

          // initialize new string and check if it was successful
          if (strInit(&token->attribute.string))
          {
            token->type = TYPE_EMPTY;
            fprintf(stderr, "Internal error when allocating string, exiting...\n");
            return SCANNER_INTERNAL;
          }

          // set new character to string
          if (strAddChar(&token->attribute.string, c))
          {
            strClear(&token->attribute.string);
            strFree(&token->attribute.string);
            fprintf(stderr, "Unable to realloc token's attribute string.\n");
            return SCANNER_INTERNAL;
          }
        }
        else if (isdigit(c))
        {
          // initialize new string and check if it was successful
          if (strInit(&token->attribute.string))
          {
            token->type = TYPE_EMPTY;
            fprintf(stderr, "Internal error when allocating string, exiting...\n");
            return SCANNER_INTERNAL;
          }

          // set new character to string
          if (strAddChar(&token->attribute.string, c))
          {
            strClear(&token->attribute.string);
            strFree(&token->attribute.string);
            fprintf(stderr, "Unable to realloc token's attribute string.\n");
            return SCANNER_INTERNAL;
          }

          state = STATE_DIGIT;
        }
        else if (c == '"')
        {
          // initialize new string and check if it was successful
          if (strInit(&token->attribute.string))
          {
            token->type = TYPE_EMPTY;
            fprintf(stderr, "Internal error when allocating string, exiting...\n");
            return SCANNER_INTERNAL;
          }

          state = STATE_STRING;
        }
        else if (c == ':')
        {
          state = STATE_DECLARATIVE_ASSIGN;
        }
        else if (c == '!')
        {
          state = STATE_NOT_EQUALS;
        }
        else if (c == '(')
        {
          if (DEBUG) printf("[(] ");
          token->type = TYPE_LEFT_BRACKET;
          return SCANNER_SUCC;
        }
        else if (c == ')')
        {
          if (DEBUG) printf("[)] ");
          token->type = TYPE_RIGHT_BRACKET;
          return SCANNER_SUCC;
        }
        else if (c == '{')
        {
          if (DEBUG) printf("[{] ");
          token->type = TYPE_LEFT_CURLY_BRACKET;
          return SCANNER_SUCC;
        }
        else if (c == '}')
        {
          token->type = TYPE_RIGHT_CURLY_BRACKET;
          if (DEBUG) printf("[}] ");
          return SCANNER_SUCC;
        }
        else if (c == ',')
        {
          if (DEBUG) printf("[,] ");
          token->type = TYPE_COMMA;
          return SCANNER_SUCC;
        }
        else if (c == '=')
        {
          if (DEBUG) printf("[=] ");
          token->type = TYPE_ASSIGN;
          return SCANNER_SUCC;
        }
        else if (c == '<')
        {
          state = STATE_LESSER_OR_EQUAL;
        }
        else if (c == '>')
        {
          state = STATE_GREATER_OR_EQUAL;
        }
        else if (c == ';')
        {
          if (DEBUG) printf("[;] ");
          token->type = TYPE_SEMICOLON;
          return SCANNER_SUCC;
        }
        else if (isspace(c))
        {
          if (c == '\n')
          {
            if (DEBUG) printf("[EOL]\n");
            token->type = TYPE_EOL;
            return SCANNER_SUCC;
          }
          continue;
        }
        // BOOLEAN EXTENSION
        else if (c == '&')
        {
          state = STATE_AND;
        }
        else if (c == '|')
        {
          state = STATE_OR;
        }
        else
        {
          fprintf(stderr, "Invalid character read, exiting...\n");
          return SCANNER_ERR;
        }
        break;

      /******   OPERATORS SECTION   ******/
      /** STATE PLUS + MINUS NEEDS TO SAVE NUMBER TO CORRESPONDING VARIABLE **/

      /** WILL INCREMENTATION AND DECREMENTATION BE IMPLEMENTED???? **/
      /** ADDED UNARY EXTENSION **/
      case STATE_PLUS:
        if (isdigit(c))
        {
          state = STATE_DIGIT;

          // initialize new string and check if it was successful
          if (strInit(&token->attribute.string))
          {
            token->type = TYPE_EMPTY;
            fprintf(stderr, "Internal error when allocating string, exiting...\n");
            return SCANNER_INTERNAL;
          }

          // set new characters to string
          if (strAddChar(&token->attribute.string, '+'))
          {
            strClear(&token->attribute.string);
            strFree(&token->attribute.string);
            fprintf(stderr, "Unable to realloc token's attribute string.\n");
            return SCANNER_INTERNAL;
          }
          if (strAddChar(&token->attribute.string, c))
          {
            strClear(&token->attribute.string);
            strFree(&token->attribute.string);
            fprintf(stderr, "Unable to realloc token's attribute string.\n");
            return SCANNER_INTERNAL;
          }
        }
        else if (c == '=')
        {
          if (DEBUG) printf("[+=] ");
          token->type = TYPE_PLUS_ASSIGN;
          return SCANNER_SUCC;
        }
        else
        {
          if (DEBUG) printf("[+] ");
          ungetc(c, stdin);
          token->type = TYPE_PLUS;
          return SCANNER_SUCC;;
        }
        break;
      case STATE_MINUS:
        if (isdigit(c))
        {
          state = STATE_DIGIT_NEGATIVE;

          // initialize new string and check if it was successful
          if (strInit(&token->attribute.string))
          {
            token->type = TYPE_EMPTY;
            fprintf(stderr, "Internal error when allocating string, exiting...\n");
            return SCANNER_INTERNAL;
          }

          // set new characters to string
          if (strAddChar(&token->attribute.string, '-'))
          {
            strClear(&token->attribute.string);
            strFree(&token->attribute.string);
            fprintf(stderr, "Unable to realloc token's attribute string.\n");
            return SCANNER_INTERNAL;
          }
          if (strAddChar(&token->attribute.string, c))
          {
            strClear(&token->attribute.string);
            strFree(&token->attribute.string);
            fprintf(stderr, "Unable to realloc token's attribute string.\n");
            return SCANNER_INTERNAL;
          }
        }
        else if (c == '=')
        {
          if (DEBUG) printf("[-=] ");
          token->type = TYPE_MINUS_ASSIGN;
          return SCANNER_SUCC;
        }
        else
        {
          if (DEBUG) printf("[-] ");
          ungetc(c, stdin);
          token->type = TYPE_MINUS;
          return SCANNER_SUCC;
        }
        break;
      case STATE_MULTIPLY:
        if (c == '=')
        {
          if (DEBUG) printf("[*=] ");
          token->type = TYPE_MULTIPLY_ASSIGN;
          return SCANNER_SUCC;
        }
        else
        {
          if (DEBUG) printf("[*] ");
          ungetc(c, stdin);
          token->type = TYPE_MULTIPLY;
          return SCANNER_SUCC;
        }
      case STATE_DIVIDE:
        if (c == '/')
        {
          state = STATE_LINE_COMMENT;
        }
        else if (c == '*')
        {
          state = STATE_MULTI_LINE_COMMENT_START;
        }
        else if (c == '=')
        {
          if (DEBUG) printf("[/=] ");
          token->type = TYPE_DIVIDE_ASSIGN;
          return SCANNER_SUCC;
        }
        else
        {
          ungetc(c, stdin);
          token->type = TYPE_DIVIDE;
          return SCANNER_SUCC;
        }
        break;
      /****** END OPERATORS SECTION ******/

      /******   ID/KEY SECTION   ******/
      case STATE_IDENTIFIER:
        if (isalpha(c) || isdigit(c) || c == '_')
        {
          // check whether adding of char was successful
          if (!strAddChar(&token->attribute.string, c)) continue;
          else
          {
            strClear(&token->attribute.string);
            strFree(&token->attribute.string);
            fprintf(stderr, "Unable to realloc token's attribute string.\n");
            return SCANNER_INTERNAL;
          }
        }
        else
        {
          if (DEBUG) printf("ID[%s] ", strGetStr(&token->attribute.string));
          ungetc(c, stdin);
          token->type = TYPE_IDENTIFIER;
          return SCANNER_SUCC;
        }
      case STATE_IDENTIFIER_OR_KEYWORD:
        if (isalpha(c))
        {
          // check whether adding of char was successful
          if (!strAddChar(&token->attribute.string, c)) continue;
          else
          {
            strClear(&token->attribute.string);
            strFree(&token->attribute.string);
            fprintf(stderr, "Unable to realloc token's attribute string.\n");
            return SCANNER_INTERNAL;
          }
        }
        else if (isdigit(c) || c == '_')
        {
          // check whether adding of char was successful
          if (!strAddChar(&token->attribute.string, c))
          {
            state = STATE_IDENTIFIER;
            continue;
          }
          else
          {
            strClear(&token->attribute.string);
            strFree(&token->attribute.string);
            fprintf(stderr, "Unable to realloc token's attribute string.\n");
            return SCANNER_INTERNAL;
          }
        }
        else
        {
          /** CHECK IF KEYWORD OR IDENTIFIER **/
          int code = isReservedKeyword(&token->attribute.string);
          if (code != 0)
          {
            if (DEBUG) printf("KEY[%s] ", strGetStr(&token->attribute.string));
            strFree(&token->attribute.string);
            setTokenKeyword(token, code);
          }
          else
          {
            if (DEBUG) printf("ID[%s] ", strGetStr(&token->attribute.string));
            token->type = TYPE_IDENTIFIER;
          }

          ungetc(c, stdin);
          return SCANNER_SUCC;
        }
      /****** END ID/KEY SECTION ******/

      /******    DATA TYPES    ******/
      case STATE_STRING:
        if (c == '"')
        {
          if (DEBUG) printf("STRING[%s] ", strGetStr(&token->attribute.string));
          token->type = TYPE_STRING;
          return SCANNER_SUCC;
        }
        else if (c == 92)
        {
          state = STATE_STRING_SKIP;
        }
        else
        {
          // check whether adding of char was successful
          if (!strAddChar(&token->attribute.string, c)) continue;
          else
          {
            strClear(&token->attribute.string);
            strFree(&token->attribute.string);
            fprintf(stderr, "Unable to realloc token's attribute string.\n");
            return SCANNER_INTERNAL;
          }
        }
        break;
      case STATE_STRING_SKIP:
      {
        if (isValidEscapeCharacter(c))
        {
          // check whether adding of char was successful
          if (strAddChar(&token->attribute.string, getEscapeSequence(c)))
          {
            strClear(&token->attribute.string);
            strFree(&token->attribute.string);
            fprintf(stderr, "Unable to realloc token's attribute string.\n");
            return SCANNER_INTERNAL;
          }

          state = STATE_STRING;
        }
        else
        {
          fprintf(stderr, "Got invalid escape sequence, exiting...");
          return SCANNER_ERR;
        }
        break;
      }

      case STATE_DIGIT:
        if (isdigit(c))
        {
          // NEEDS FIX
          if (token->attribute.integer == 0)
          {
            fprintf(stderr, "No leading zeros are supported, exiting...\n");
            return SCANNER_ERR;
          }

          // set new character to string
          if (strAddChar(&token->attribute.string, c))
          {
            strClear(&token->attribute.string);
            strFree(&token->attribute.string);
            fprintf(stderr, "Unable to realloc token's attribute string.\n");
            return SCANNER_INTERNAL;
          }
        }
        else if (c == 'e' || c == 'E')
        {
          // set new character to string
          if (strAddChar(&token->attribute.string, c))
          {
            strClear(&token->attribute.string);
            strFree(&token->attribute.string);
            fprintf(stderr, "Unable to realloc token's attribute string.\n");
            return SCANNER_INTERNAL;
          }

          state = STATE_DIGIT_WITH_EXP;
        }
        else if (c == '.')
        {
          // set new character to string
          if (strAddChar(&token->attribute.string, c))
          {
            strClear(&token->attribute.string);
            strFree(&token->attribute.string);
            fprintf(stderr, "Unable to realloc token's attribute string.\n");
            return SCANNER_INTERNAL;
          }

          state = STATE_FLOAT;
        }
        else
        {
          ungetc(c, stdin);

          // change from string to int and clear string
          int64_t tmp = atoll(strGetStr(&token->attribute.string));
          strClear(&token->attribute.string);
          strFree(&token->attribute.string);
          token->attribute.integer = tmp;

          if (DEBUG) printf("INT[%lld] ", (long long) token->attribute.integer);
          token->type = TYPE_INT;
          return SCANNER_SUCC;
        }
        break;
      case STATE_DIGIT_WITH_EXP:
        if (isdigit(c) || c == '+' || c == '-')
        {
          // set new character to string
          if (strAddChar(&token->attribute.string, c))
          {
            strClear(&token->attribute.string);
            strFree(&token->attribute.string);
            fprintf(stderr, "Unable to realloc token's attribute string.\n");
            return SCANNER_INTERNAL;
          }

          state = STATE_DIGIT_WITH_EXP_AND_OP;
        }
        else
        {
          fprintf(stderr, "Invalid format for number with exponent, exiting...\n");
          return SCANNER_ERR;
        }
        break;
      case STATE_DIGIT_WITH_EXP_AND_OP:
        if (isdigit(c))
        {
          // set new character to string
          if (strAddChar(&token->attribute.string, c))
          {
            strClear(&token->attribute.string);
            strFree(&token->attribute.string);
            fprintf(stderr, "Unable to realloc token's attribute string.\n");
            return SCANNER_INTERNAL;
          }
        }
        else
        {
          ungetc(c, stdin);

          // change from string to int and clear string
          int64_t tmp = strtod(strGetStr(&token->attribute.string), NULL);
          strClear(&token->attribute.string);
          strFree(&token->attribute.string);
          token->attribute.integer = tmp;

          if (DEBUG) printf("INT[%lld] ", (long long) token->attribute.integer);
          token->type = TYPE_INT;
          return SCANNER_SUCC;
        }
        break;

      case STATE_DIGIT_NEGATIVE:
        if (isdigit(c))
        {
          if (token->attribute.integer == 0)
          {
            fprintf(stderr, "No leading zeros are supported, exiting...\n");
            return SCANNER_ERR;
          }

          // set new character to string
          if (strAddChar(&token->attribute.string, c))
          {
            strClear(&token->attribute.string);
            strFree(&token->attribute.string);
            fprintf(stderr, "Unable to realloc token's attribute string.\n");
            return SCANNER_INTERNAL;
          }
        }
        else if (c == 'e' || c == 'E')
        {
          // set new character to string
          if (strAddChar(&token->attribute.string, c))
          {
            strClear(&token->attribute.string);
            strFree(&token->attribute.string);
            fprintf(stderr, "Unable to realloc token's attribute string.\n");
            return SCANNER_INTERNAL;
          }

          state = STATE_DIGIT_NEGATIVE_WITH_EXP;
        }
        else if (c == '.')
        {
          // set new character to string
          if (strAddChar(&token->attribute.string, c))
          {
            strClear(&token->attribute.string);
            strFree(&token->attribute.string);
            fprintf(stderr, "Unable to realloc token's attribute string.\n");
            return SCANNER_INTERNAL;
          }

          state = STATE_FLOAT_NEGATIVE;
        }
        else
        {
          ungetc(c, stdin);

          // change from string to float and clear string
          int64_t tmp = atoll(strGetStr(&token->attribute.string));
          strClear(&token->attribute.string);
          strFree(&token->attribute.string);
          token->attribute.integer = tmp;

          if (DEBUG) printf("INT[%lld] ", (long long) token->attribute.integer);
          token->type = TYPE_INT;
          return SCANNER_SUCC;
        }
        break;
      case STATE_DIGIT_NEGATIVE_WITH_EXP:
        if (isdigit(c) || c == '+' || c == '-')
        {
          // set new character to string
          if (strAddChar(&token->attribute.string, c))
          {
            strClear(&token->attribute.string);
            strFree(&token->attribute.string);
            fprintf(stderr, "Unable to realloc token's attribute string.\n");
            return SCANNER_INTERNAL;
          }

          state = STATE_DIGIT_NEGATIVE_WITH_EXP_AND_OP;
        }
        else
        {
          fprintf(stderr, "Invalid format for number with exponent, exiting...\n");
          return SCANNER_ERR;
        }
        break;
      case STATE_DIGIT_NEGATIVE_WITH_EXP_AND_OP:
        if (isdigit(c))
        {
          // set new character to string
          if (strAddChar(&token->attribute.string, c))
          {
            strClear(&token->attribute.string);
            strFree(&token->attribute.string);
            fprintf(stderr, "Unable to realloc token's attribute string.\n");
            return SCANNER_INTERNAL;
          }
        }
        else
        {
          ungetc(c, stdin);

          // change from string to int and clear string
          int64_t tmp = strtod(strGetStr(&token->attribute.string), NULL);
          strClear(&token->attribute.string);
          strFree(&token->attribute.string);
          token->attribute.integer = tmp;

          if (DEBUG) printf("INT[%lld] ", (long long) token->attribute.integer);
          token->type = TYPE_INT;
          return SCANNER_SUCC;
        }
        break;

      case STATE_FLOAT:
        if (isdigit(c))
        {
          // set new character to string
          if (strAddChar(&token->attribute.string, c))
          {
            strClear(&token->attribute.string);
            strFree(&token->attribute.string);
            fprintf(stderr, "Unable to realloc token's attribute string.\n");
            return SCANNER_INTERNAL;
          }
        }
        else if (c == 'e' || c == 'E')
        {
          // set new character to string
          if (strAddChar(&token->attribute.string, c))
          {
            strClear(&token->attribute.string);
            strFree(&token->attribute.string);
            fprintf(stderr, "Unable to realloc token's attribute string.\n");
            return SCANNER_INTERNAL;
          }

          state = STATE_FLOAT_WITH_EXP;
        }
        else
        {
          ungetc(c, stdin);

          // change from string to float and clear string
          float tmp = strtod(strGetStr(&token->attribute.string), NULL);
          strClear(&token->attribute.string);
          strFree(&token->attribute.string);
          token->attribute.float64 = tmp;

          if (DEBUG) printf("FLOAT[%f] ", token->attribute.float64);
          token->type = TYPE_FLOAT64;
          return SCANNER_SUCC;
        }
        break;
      case STATE_FLOAT_WITH_EXP:
        if (isdigit(c) || c == '+' || c == '-')
        {
          // set new character to string
          if (strAddChar(&token->attribute.string, c))
          {
            strClear(&token->attribute.string);
            strFree(&token->attribute.string);
            fprintf(stderr, "Unable to realloc token's attribute string.\n");
            return SCANNER_INTERNAL;
          }

          state = STATE_FLOAT_WITH_EXP_AND_OP;
        }
        else
        {
          fprintf(stderr, "Invalid format for float with exponent, exiting...\n");
          return SCANNER_ERR;
        }
        break;
      case STATE_FLOAT_WITH_EXP_AND_OP:
        if (isdigit(c))
        {
          // set new character to string
          if (strAddChar(&token->attribute.string, c))
          {
            strClear(&token->attribute.string);
            strFree(&token->attribute.string);
            fprintf(stderr, "Unable to realloc token's attribute string.\n");
            return SCANNER_INTERNAL;
          }
        }
        else
        {
          ungetc(c, stdin);

          // change from string to int and clear string
          float tmp = strtod(strGetStr(&token->attribute.string), NULL);
          strClear(&token->attribute.string);
          strFree(&token->attribute.string);
          token->attribute.float64 = tmp;

          if (DEBUG) printf("FLOAT[%f] ", token->attribute.float64);
          token->type = TYPE_INT;
          return SCANNER_SUCC;
        }
        break;

      case STATE_FLOAT_NEGATIVE:
        if (isdigit(c))
        {
          // set new character to string
          if (strAddChar(&token->attribute.string, c))
          {
            strClear(&token->attribute.string);
            strFree(&token->attribute.string);
            fprintf(stderr, "Unable to realloc token's attribute string.\n");
            return SCANNER_INTERNAL;
          }
        }
        else if (c == 'e' || c == 'E')
        {
          // set new character to string
          if (strAddChar(&token->attribute.string, c))
          {
            strClear(&token->attribute.string);
            strFree(&token->attribute.string);
            fprintf(stderr, "Unable to realloc token's attribute string.\n");
            return SCANNER_INTERNAL;
          }

          state = STATE_FLOAT_WITH_EXP;
        }
        else
        {
          ungetc(c, stdin);

          // change from string to float and clear string
          float tmp = strtod(strGetStr(&token->attribute.string), NULL);
          strClear(&token->attribute.string);
          strFree(&token->attribute.string);
          token->attribute.float64 = tmp;

          if (DEBUG) printf("FLOAT[%f] ", token->attribute.float64);
          token->type = TYPE_FLOAT64;
          return SCANNER_SUCC;
        }
        break;
      case STATE_FLOAT_NEGATIVE_WITH_EXP:
        if (isdigit(c) || c == '+' || c == '-')
        {
          // set new character to string
          if (strAddChar(&token->attribute.string, c))
          {
            strClear(&token->attribute.string);
            strFree(&token->attribute.string);
            fprintf(stderr, "Unable to realloc token's attribute string.\n");
            return SCANNER_INTERNAL;
          }

          state = STATE_FLOAT_NEGATIVE_WITH_EXP_AND_OP;
        }
        else
        {
          fprintf(stderr, "Invalid format for float with exponent, exiting...\n");
          return SCANNER_ERR;
        }
        break;
      case STATE_FLOAT_NEGATIVE_WITH_EXP_AND_OP:
        if (isdigit(c))
        {
          // set new character to string
          if (strAddChar(&token->attribute.string, c))
          {
            strClear(&token->attribute.string);
            strFree(&token->attribute.string);
            fprintf(stderr, "Unable to realloc token's attribute string.\n");
            return SCANNER_INTERNAL;
          }
        }
        else
        {
          ungetc(c, stdin);

          // change from string to int and clear string
          float tmp = strtod(strGetStr(&token->attribute.string), NULL);
          strClear(&token->attribute.string);
          strFree(&token->attribute.string);
          token->attribute.float64 = tmp;

          if (DEBUG) printf("FLOAT[%f] ", token->attribute.float64);
          token->type = TYPE_INT;
          return SCANNER_SUCC;
        }
        break;
      /****** END DATA TYPES ******/

      /******   COMMENTS SECTION   ******/
      case STATE_LINE_COMMENT:
        if (c == '\n' || c == EOF)
        {
          if (DEBUG) printf("[Line Comment] ");
          ungetc(c, stdin);

          token->type = TYPE_EMPTY;
          return SCANNER_SUCC;
        }
        break;
      case STATE_MULTI_LINE_COMMENT_START:
        if (c == '*' || c == EOF)
        {
          state = STATE_MULTI_LINE_COMMENT_END;
        }
        break;
      case STATE_MULTI_LINE_COMMENT_END:
        if (c == '/' || c == EOF)
        {
          if (DEBUG) printf("[Multi line comment] ");
          token->type = TYPE_EMPTY;
          return SCANNER_SUCC;
        }
        else
        {
          state = STATE_MULTI_LINE_COMMENT_START;
        }
        break;
      /****** END COMMENTS SECTION ******/

      case STATE_DECLARATIVE_ASSIGN:
        if (c == '=')
        {
          if (DEBUG) printf("[:=] ");
          token->type = TYPE_DECLARATIVE_ASSIGN;
          return SCANNER_SUCC;
        }
        else
        {
          fprintf(stderr, "Didn't get '=' for declarative assign ':=', exiting...\n");
          return SCANNER_ERR;
        }
      case STATE_NOT_EQUALS:
        if (c == '=')
        {
          if (DEBUG) printf("[!=] ");
          token->type = TYPE_NOT_EQUALS;
          return SCANNER_SUCC;
        }
        // BOOLEAN EXTENSION [without it should return 1 for missing '=']
        else
        {
          if (DEBUG) printf("[!] ");
          token->type = TYPE_NOT;
          return SCANNER_SUCC;
        }
        break;
      case STATE_GREATER_OR_EQUAL:
        if (c == '=')
        {
          if (DEBUG) printf("[>=] ");
          token->type = TYPE_GREATER_OR_EQUAL;
          return SCANNER_SUCC;
        }
        else
        {
          if (DEBUG) printf("[>] ");
          ungetc(c, stdin);
          token->type = TYPE_GREATER;
          return SCANNER_SUCC;
        }
      case STATE_LESSER_OR_EQUAL:
        if (c == '=')
        {
          if (DEBUG) printf("[<=] ");
          token->type = TYPE_LESSER_OR_EQUAL;
          return SCANNER_SUCC;
        }
        else
        {
          if (DEBUG) printf("[<] ");
          ungetc(c, stdin);
          token->type = TYPE_LESSER;
          return SCANNER_SUCC;
        }

      /******   BOOLEAN EXTENSION   ******/
      case STATE_AND:
        if (c == '&')
        {
          if (DEBUG) printf("[&&] ");
          token->type = TYPE_AND;
          return SCANNER_SUCC;
        }
        else
        {
          fprintf(stderr, "Didn't get '&' for AND, exiting...\n");
          return SCANNER_ERR;
        }
      case STATE_OR:
        if (c == '|')
        {
          if (DEBUG) printf("[||] ");
          token->type = TYPE_OR;
          return SCANNER_SUCC;
        }
        else
        {
          fprintf(stderr, "Didn't get '|' for OR, exiting...\n");
          return SCANNER_ERR;
        }
      /****** END BOOLEAN EXTENSION ******/
    }
  }
}

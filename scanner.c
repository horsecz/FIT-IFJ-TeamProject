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

FILE *sourceFile = NULL;


int setSourceFile (FILE *file)
{
  if (file != NULL)
  {
    sourceFile = file;
    return 0;
  }
  return 1;
}

void unsetSourceFile ()
{
  sourceFile = NULL;
}

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
    return 2;
  }

  // char read from @var sourceFile
  char c;
  // initialization of state
  State state = STATE_START;

  while (1)
  {
    // read another character
    c = getc(sourceFile);

    // enter deterministic automaton
    switch (state)
    {
      // TODO: Retezcovy literal specifikace escape sequence
      case STATE_START:
        if (c == EOF)
        {
          printf("[EOF]\n");
          token->type = TYPE_EOF;
          return 0;
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
            printf("Internal error when allocating string, exiting...\n");
            return 99;
          }

          // set new character to string
          if (strAddChar(&token->attribute.string, c))
          {
            strClear(&token->attribute.string);
            strFree(&token->attribute.string);
            printf("Unable to realloc token's attribute string.\n");
            return 99;
          }
        }
        else if (isdigit(c))
        {
          // initialize new string and check if it was successful
          if (strInit(&token->attribute.string))
          {
            token->type = TYPE_EMPTY;
            printf("Internal error when allocating string, exiting...\n");
            return 99;
          }

          // set new character to string
          if (strAddChar(&token->attribute.string, c))
          {
            strClear(&token->attribute.string);
            strFree(&token->attribute.string);
            printf("Unable to realloc token's attribute string.\n");
            return 99;
          }

          state = STATE_DIGIT;
        }
        else if (c == '"')
        {
          // initialize new string and check if it was successful
          if (strInit(&token->attribute.string))
          {
            token->type = TYPE_EMPTY;
            printf("Internal error when allocating string, exiting...\n");
            return 99;
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
          printf("[(] ");
          token->type = TYPE_LEFT_BRACKET;
          return 0;
        }
        else if (c == ')')
        {
          printf("[)] ");
          token->type = TYPE_RIGHT_BRACKET;
          return 0;
        }
        else if (c == '{')
        {
          printf("[{] ");
          token->type = TYPE_LEFT_CURLY_BRACKET;
          return 0;
        }
        else if (c == '}')
        {
          token->type = TYPE_RIGHT_CURLY_BRACKET;
          printf("[}] ");
          return 0;
        }
        else if (c == ',')
        {
          printf("[,] ");
          token->type = TYPE_COMMA;
          return 0;
        }
        else if (c == '=')
        {
          printf("[=] ");
          token->type = TYPE_ASSIGN;
          return 0;
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
          printf("[;] ");
          token->type = TYPE_SEMICOLON;
          return 0;
        }
        else if (isspace(c))
        {
          if (c == '\n')
          {
            printf("[EOL]\n");
            token->type = TYPE_EOL;
            return 0;
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
          printf("Invalid character read, exiting...\n");
          return 1;
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
            printf("Internal error when allocating string, exiting...\n");
            return 99;
          }

          // set new characters to string
          if (strAddChar(&token->attribute.string, '+'))
          {
            strClear(&token->attribute.string);
            strFree(&token->attribute.string);
            printf("Unable to realloc token's attribute string.\n");
            return 99;
          }
          if (strAddChar(&token->attribute.string, c))
          {
            strClear(&token->attribute.string);
            strFree(&token->attribute.string);
            printf("Unable to realloc token's attribute string.\n");
            return 99;
          }
        }
        else if (c == '=')
        {
          printf("[+=] ");
          token->type = TYPE_PLUS_ASSIGN;
          return 0;
        }
        else
        {
          printf("[+] ");
          ungetc(c, sourceFile);
          token->type = TYPE_PLUS;
          return 0;
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
            printf("Internal error when allocating string, exiting...\n");
            return 99;
          }

          // set new characters to string
          if (strAddChar(&token->attribute.string, '-'))
          {
            strClear(&token->attribute.string);
            strFree(&token->attribute.string);
            printf("Unable to realloc token's attribute string.\n");
            return 99;
          }
          if (strAddChar(&token->attribute.string, c))
          {
            strClear(&token->attribute.string);
            strFree(&token->attribute.string);
            printf("Unable to realloc token's attribute string.\n");
            return 99;
          }
        }
        else if (c == '=')
        {
          printf("[-=] ");
          token->type = TYPE_MINUS_ASSIGN;
          return 0;
        }
        else
        {
          printf("[-] ");
          ungetc(c, sourceFile);
          token->type = TYPE_MINUS;
          return 0;
        }
        break;
      case STATE_MULTIPLY:
        if (c == '=')
        {
          printf("[*=] ");
          token->type = TYPE_MULTIPLY_ASSIGN;
          return 0;
        }
        else
        {
          printf("[*] ");
          ungetc(c, sourceFile);
          token->type = TYPE_MULTIPLY;
          return 0;
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
          printf("[/=] ");
          token->type = TYPE_DIVIDE_ASSIGN;
          return 0;
        }
        else
        {
          ungetc(c, sourceFile);
          token->type = TYPE_DIVIDE;
          return 0;
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
            printf("Unable to realloc token's attribute string.\n");
            return 99;
          }
        }
        else
        {
          printf("ID[%s] ", strGetStr(&token->attribute.string));
          ungetc(c, sourceFile);
          token->type = TYPE_IDENTIFIER;
          return 0;
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
            printf("Unable to realloc token's attribute string.\n");
            return 99;
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
            printf("Unable to realloc token's attribute string.\n");
            return 99;
          }
        }
        else
        {
          /** CHECK IF KEYWORD OR IDENTIFIER **/
          int code = isReservedKeyword(&token->attribute.string);
          if (code != 0)
          {
            printf("KEY[%s] ", strGetStr(&token->attribute.string));
            setTokenKeyword(token, code);
          }
          else
          {
            printf("ID[%s] ", strGetStr(&token->attribute.string));
            token->type = TYPE_IDENTIFIER;
          }

          ungetc(c, sourceFile);
          return 0;
        }
      /****** END ID/KEY SECTION ******/

      /******    DATA TYPES    ******/
      case STATE_STRING:
        if (c == '"')
        {
          printf("STRING[%s] ", strGetStr(&token->attribute.string));
          token->type = TYPE_STRING;
          return 0;
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
            printf("Unable to realloc token's attribute string.\n");
            return 99;
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
            printf("Unable to realloc token's attribute string.\n");
            return 99;
          }

          state = STATE_STRING;
        }
        else
        {
          printf("Get invalid escape sequence, exiting...");
          return 1;
        }
        break;
      }

      case STATE_DIGIT:
        if (isdigit(c))
        {
          // NEEDS FIX
          if (token->attribute.integer == 0)
          {
            printf("No leading zeros are supported, exiting...\n");
            return 1;
          }

          // set new character to string
          if (strAddChar(&token->attribute.string, c))
          {
            strClear(&token->attribute.string);
            strFree(&token->attribute.string);
            printf("Unable to realloc token's attribute string.\n");
            return 99;
          }
        }
        else if (c == 'e' || c == 'E')
        {
          // set new character to string
          if (strAddChar(&token->attribute.string, c))
          {
            strClear(&token->attribute.string);
            strFree(&token->attribute.string);
            printf("Unable to realloc token's attribute string.\n");
            return 99;
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
            printf("Unable to realloc token's attribute string.\n");
            return 99;
          }

          state = STATE_FLOAT;
        }
        else
        {
          ungetc(c, sourceFile);

          // change from string to int and clear string
          int64_t tmp = atoll(strGetStr(&token->attribute.string));
          strClear(&token->attribute.string);
          strFree(&token->attribute.string);
          token->attribute.integer = tmp;

          printf("INT[%lld] ", (long long) token->attribute.integer);
          token->type = TYPE_INT;
          return 0;
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
            printf("Unable to realloc token's attribute string.\n");
            return 99;
          }

          state = STATE_DIGIT_WITH_EXP_AND_OP;
        }
        else
        {
          printf("Invalid format for number with exponent, exiting...\n");
          return 1;
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
            printf("Unable to realloc token's attribute string.\n");
            return 99;
          }
        }
        else
        {
          ungetc(c, sourceFile);

          // change from string to int and clear string
          int64_t tmp = strtod(strGetStr(&token->attribute.string), NULL);
          strClear(&token->attribute.string);
          strFree(&token->attribute.string);
          token->attribute.integer = tmp;

          printf("INT[%lld] ", (long long) token->attribute.integer);
          token->type = TYPE_INT;
          return 0;
        }
        break;

      case STATE_DIGIT_NEGATIVE:
        if (isdigit(c))
        {
          if (token->attribute.integer == 0)
          {
            printf("No leading zeros are supported, exiting...\n");
            return 1;
          }

          // set new character to string
          if (strAddChar(&token->attribute.string, c))
          {
            strClear(&token->attribute.string);
            strFree(&token->attribute.string);
            printf("Unable to realloc token's attribute string.\n");
            return 99;
          }
        }
        else if (c == 'e' || c == 'E')
        {
          // set new character to string
          if (strAddChar(&token->attribute.string, c))
          {
            strClear(&token->attribute.string);
            strFree(&token->attribute.string);
            printf("Unable to realloc token's attribute string.\n");
            return 99;
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
            printf("Unable to realloc token's attribute string.\n");
            return 99;
          }

          state = STATE_FLOAT_NEGATIVE;
        }
        else
        {
          ungetc(c, sourceFile);

          // change from string to float and clear string
          int64_t tmp = atoll(strGetStr(&token->attribute.string));
          strClear(&token->attribute.string);
          strFree(&token->attribute.string);
          token->attribute.integer = tmp;

          printf("INT[%lld] ", (long long) token->attribute.integer);
          token->type = TYPE_INT;
          return 0;
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
            printf("Unable to realloc token's attribute string.\n");
            return 99;
          }

          state = STATE_DIGIT_NEGATIVE_WITH_EXP_AND_OP;
        }
        else
        {
          printf("Invalid format for number with exponent, exiting...\n");
          return 1;
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
            printf("Unable to realloc token's attribute string.\n");
            return 99;
          }
        }
        else
        {
          ungetc(c, sourceFile);

          // change from string to int and clear string
          int64_t tmp = strtod(strGetStr(&token->attribute.string), NULL);
          strClear(&token->attribute.string);
          strFree(&token->attribute.string);
          token->attribute.integer = tmp;

          printf("INT[%lld] ", (long long) token->attribute.integer);
          token->type = TYPE_INT;
          return 0;
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
            printf("Unable to realloc token's attribute string.\n");
            return 99;
          }
        }
        else if (c == 'e' || c == 'E')
        {
          // set new character to string
          if (strAddChar(&token->attribute.string, c))
          {
            strClear(&token->attribute.string);
            strFree(&token->attribute.string);
            printf("Unable to realloc token's attribute string.\n");
            return 99;
          }

          state = STATE_FLOAT_WITH_EXP;
        }
        else
        {
          ungetc(c, sourceFile);

          // change from string to float and clear string
          float tmp = strtod(strGetStr(&token->attribute.string), NULL);
          strClear(&token->attribute.string);
          strFree(&token->attribute.string);
          token->attribute.float64 = tmp;

          printf("FLOAT[%f] ", token->attribute.float64);
          token->type = TYPE_FLOAT64;
          return 0;
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
            printf("Unable to realloc token's attribute string.\n");
            return 99;
          }

          state = STATE_FLOAT_WITH_EXP_AND_OP;
        }
        else
        {
          printf("Invalid format for float with exponent, exiting...\n");
          return 1;
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
            printf("Unable to realloc token's attribute string.\n");
            return 99;
          }
        }
        else
        {
          ungetc(c, sourceFile);

          // change from string to int and clear string
          float tmp = strtod(strGetStr(&token->attribute.string), NULL);
          strClear(&token->attribute.string);
          strFree(&token->attribute.string);
          token->attribute.float64 = tmp;

          printf("FLOAT[%f] ", token->attribute.float64);
          token->type = TYPE_INT;
          return 0;
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
            printf("Unable to realloc token's attribute string.\n");
            return 99;
          }
        }
        else if (c == 'e' || c == 'E')
        {
          // set new character to string
          if (strAddChar(&token->attribute.string, c))
          {
            strClear(&token->attribute.string);
            strFree(&token->attribute.string);
            printf("Unable to realloc token's attribute string.\n");
            return 99;
          }

          state = STATE_FLOAT_WITH_EXP;
        }
        else
        {
          ungetc(c, sourceFile);

          // change from string to float and clear string
          float tmp = strtod(strGetStr(&token->attribute.string), NULL);
          strClear(&token->attribute.string);
          strFree(&token->attribute.string);
          token->attribute.float64 = tmp;

          printf("FLOAT[%f] ", token->attribute.float64);
          token->type = TYPE_FLOAT64;
          return 0;
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
            printf("Unable to realloc token's attribute string.\n");
            return 99;
          }

          state = STATE_FLOAT_NEGATIVE_WITH_EXP_AND_OP;
        }
        else
        {
          printf("Invalid format for float with exponent, exiting...\n");
          return 1;
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
            printf("Unable to realloc token's attribute string.\n");
            return 99;
          }
        }
        else
        {
          ungetc(c, sourceFile);

          // change from string to int and clear string
          float tmp = strtod(strGetStr(&token->attribute.string), NULL);
          strClear(&token->attribute.string);
          strFree(&token->attribute.string);
          token->attribute.float64 = tmp;

          printf("FLOAT[%f] ", token->attribute.float64);
          token->type = TYPE_INT;
          return 0;
        }
        break;
      /****** END DATA TYPES ******/

      /******   COMMENTS SECTION   ******/
      case STATE_LINE_COMMENT:
        if (c == '\n' || c == EOF)
        {
          printf("[Line Comment] ");
          ungetc(c, sourceFile);

          token->type = TYPE_EMPTY;
          return 0;
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
          printf("[Multi line comment] ");
          token->type = TYPE_EMPTY;
          return 0;
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
          printf("[:=] ");
          token->type = TYPE_DECLARATIVE_ASSIGN;
          return 0;
        }
        else
        {
          printf("Didn't get '=' for declarative assign ':=', exiting...\n");
          return 1;
        }
      case STATE_NOT_EQUALS:
        if (c == '=')
        {
          printf("[!=] ");
          token->type = TYPE_NOT_EQUALS;
          return 0;
        }
        // BOOLEAN EXTENSION [without it should return 1 for missing '=']
        else
        {
          printf("[!] ");
          token->type = TYPE_NOT;
          return 0;
        }
        break;
      case STATE_GREATER_OR_EQUAL:
        if (c == '=')
        {
          printf("[>=] ");
          token->type = TYPE_GREATER_OR_EQUAL;
          return 0;
        }
        else
        {
          printf("[>] ");
          ungetc(c, sourceFile);
          token->type = TYPE_GREATER;
          return 0;
        }
      case STATE_LESSER_OR_EQUAL:
        if (c == '=')
        {
          printf("[<=] ");
          token->type = TYPE_LESSER_OR_EQUAL;
          return 0;
        }
        else
        {
          printf("[<] ");
          ungetc(c, sourceFile);
          token->type = TYPE_LESSER;
          return 0;
        }

      /******   BOOLEAN EXTENSION   ******/
      case STATE_AND:
        if (c == '&')
        {
          printf("[&&] ");
          token->type = TYPE_AND;
          return 0;
        }
        else
        {
          printf("Didn't get '&' for AND, exiting...\n");
          return 1;
        }
      case STATE_OR:
        if (c == '|')
        {
          printf("[||] ");
          token->type = TYPE_OR;
          return 0;
        }
        else
        {
          printf("Didn't get '|' for OR, exiting...\n");
          return 1;
        }
      /****** END BOOLEAN EXTENSION ******/
    }
  }
}

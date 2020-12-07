/**
 * @file returns.c
 * @author Roman Janiczek (xjanic25@vutbr.cz)
 * @brief Implementation of helper functions
 * @version 0.1
 * @date 2020-11-14
 */
#include "returns.h"
#define TOKEN_CHAR_MAX 255

char *errorString[] = {
    "SUCCESS",
    "LEXICAL ANALYSIS",
    "SYNTAX ANALYSIS",
    "SEMANTIC (UNDEFINED VALUES, REDEFINITON ATTEMPTS, ETC.)",
    "SEMANTIC (TYPE ASSIGN)",
    "SEMANTIC (TYPE COMPATIBILITY)",
    "SEMANTIC (PARAMETRS & RETURN VALUES)",
    "SEMANTIC (OTHER)",
    "",
    "SEMANTIC (ZERO DIVISION)"
    "INTERNAL",
    "WARNING"
};

void iPrint(eRC code, bool error, char* opMsg) {
    fprintf(stderr, (!opMsg) ? "%s: %s\n" : "%s: %s (%s)\n", (error) ? "[ERROR]" : "[INFO]",
    (code == RC_ERR_INTERNAL || code == RC_WRN_INTERNAL) ? 
    ((code == RC_ERR_INTERNAL) ? errorString[10] : errorString[11]) : errorString[code], 
    (opMsg != NULL) ? opMsg : NULL);
}

void tokenToString(Token* tk, string* str) {
    char add[TOKEN_CHAR_MAX] = { 0 };

    if (tk->type == TYPE_KEYWORD) {
        switch(tk->attribute.keyword) {
            case KEYWORD_PACKAGE:
                strcpy(add, "package");
                break;
            case KEYWORD_IMPORT:
                strcpy(add, "import");
                break;
            case KEYWORD_FUNC:
                strcpy(add, "func");
                break;
            case KEYWORD_RETURN:
                strcpy(add, "return");
                break;
            case KEYWORD_VAR:
                strcpy(add, "var");
                break;
            case KEYWORD_CONST:
                strcpy(add, "const");
                break;
            case KEYWORD_IF:
                strcpy(add, "if");
                break;
            case KEYWORD_ELSE:
                strcpy(add, "else");
                break;
            case KEYWORD_SWITCH:
                strcpy(add, "switch");
                break;
            case KEYWORD_CASE:
                strcpy(add, "case");
                break;
            case KEYWORD_DEFAULT:
                strcpy(add, "default");
                break;
            case KEYWORD_FOR:
                strcpy(add, "for");
                break;
            case KEYWORD_STRING:
                strcpy(add, "string");
                break;
            case KEYWORD_INT:
                strcpy(add, "int");
                break;
            case KEYWORD_FLOAT64:
                strcpy(add, "float64");
                break;
            case KEYWORD_BOOL:
                strcpy(add, "bool");
                break;
            default:
                return;
                break;
        }
    } else if (tk->type == TYPE_IDENTIFIER) {
        strncpy(add, strGetStr(&tk->attribute.string), TOKEN_CHAR_MAX);
    } else {
        switch(tk->type) {
            case TYPE_DIVIDE_ASSIGN:
                strAddChar(str, '/');
                strAddChar(str, '=');
                break;
            case TYPE_MULTIPLY_ASSIGN:
                strAddChar(str, '*');
                strAddChar(str, '=');
                break;
            case TYPE_MINUS_ASSIGN:
                strAddChar(str, '-');
                strAddChar(str, '=');
                break;
            case TYPE_PLUS_ASSIGN:
                strAddChar(str, '+');
                strAddChar(str, '=');
                break;
            case TYPE_DECLARATIVE_ASSIGN:
                strAddChar(str, ':');
                strAddChar(str, '=');
                break;
            case TYPE_RIGHT_CURLY_BRACKET:
                strAddChar(str, '}');
                break;
            case TYPE_LEFT_CURLY_BRACKET:
                strAddChar(str, '{');
                break;
            case TYPE_ASSIGN:
                strAddChar(str, '=');
                break;
            case TYPE_SEMICOLON:
                strAddChar(str, ';');
                break;
            case TYPE_COMMA:
                strAddChar(str, ':');
                break;
            case TYPE_EQUALS:
                strAddChar(str, '=');
                strAddChar(str, '=');
                break;
            case TYPE_RIGHT_BRACKET:
                strAddChar(str, ')');
                break;
            case TYPE_EOL:
                strcpy(add, "EOL");
                break;
            case TYPE_LEFT_BRACKET:
                strAddChar(str, '(');
                break;
            case TYPE_STRING:
                strcpy(add, "string");
                break;
            case TYPE_FLOAT64:
                strcpy(add, "float64");
                break;
            case TYPE_INT:
                strcpy(add, "int");
                break;
            case TYPE_EOF:
                strcpy(add, "EOF");
                break;
            case TYPE_EMPTY:
                strcpy(add, "Comment");
                break;
            case TYPE_AND:
                strAddChar(str, '&');
                break;
            case TYPE_BOOL:
                strcpy(add, "bool");
                break;
            case TYPE_DIVIDE:
                strAddChar(str, '/');
                break;
            case TYPE_GREATER:
                strAddChar(str, '>');
                break;
            case TYPE_GREATER_OR_EQUAL:
                strAddChar(str, '>');
                strAddChar(str, '=');
                break;
            case TYPE_LESSER:
                strAddChar(str, '<');
                break;
            case TYPE_LESSER_OR_EQUAL:
                strAddChar(str, '<');
                strAddChar(str, '=');
                break;
            case TYPE_MINUS:
                strAddChar(str, '-');
                break;
            case TYPE_MODULO_ASSIGN:
                strAddChar(str, '%');
                strAddChar(str, '=');
                break;
            case TYPE_MULTIPLY:
                strAddChar(str, '*');
                break;
            case TYPE_NOT:
                strAddChar(str, '!');
                break;
            case TYPE_NOT_EQUALS:
                strAddChar(str, '!');
                strAddChar(str, '=');
                break;
            case TYPE_OR:
                strAddChar(str, '|');
                break;
            case TYPE_PLUS:
                strAddChar(str, '+');
                break;
            default:
                return;
                break;
        }
    }

    for (int i = 0; add[i] != '\0'; i++) {
        strAddChar(str, add[i]);
    }

    return;
}
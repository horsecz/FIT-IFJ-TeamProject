#ifndef _PRECEDENT_H
#define _PRECEDENT_H

#define SCANNER_OK      0
#define SYNTAX_OK       0
#define SCANNER_ERROR   9   
#define SYNTAX_ERROR    9
#define SEM_ERROR_DEF 	9
#define SEM_ERROR_TYPE  9
#define SEM_ERROR_PARAM 9
#define SEM_ERROR_OTHER 9
#define ZERO_DIVISION   9
#define TOKEN_ERROR     9
#define INTERNAL_ERROR  99

#include <stdio.h>
#include <stdlib.h>

#include "scanner.h"
#include "symtable.h"
#include "symstack.h"
#include "error.h"
#include "string.h"

int precedent_analys(Token* help, Token* tokeng, TokenType* Type);

int idkfunkce(symStack *stack, Token* token, TokenType* Type);

TokenType StackTopTerm (symStack *stack);

int stackPushOpen(symStack *stack); 

symStackItem* stackPosition(symStack *stack, int j);

int reduction(symStack *stack);

void setInFunction(bool in);

#endif

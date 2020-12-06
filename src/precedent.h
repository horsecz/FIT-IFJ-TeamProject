

#ifndef _PRECEDENT_H
#define _PRECEDENT_H

#define SCANNER_OK      0
#define SYNTAX_OK       0
#define SCANNER_ERROR   1   
#define SYNTAX_ERROR    2
#define SEM_ERROR_DEF 	3
#define SEM_ERROR_TYPE  4
#define SEM_ERROR_TYPECOMP 5
#define SEM_ERROR_PARAM 6
#define SEM_ERROR_OTHER 7
#define ZERO_DIVISION   9
#define INTERNAL_ERROR  99

#include <stdio.h>
#include <stdlib.h>

#include "scanner.h"
#include "symtable.h"
#include "symstack.h"
#include "error.h"
#include "string.h"

int precedent_analys(Token* tokeng, TokenType* Type, stStack* Vars);

int idkfunkce(symStack *stack, Token* token, TokenType* Type, stStack* Vars);

TokenType StackTopTerm (symStack *stack);

int stackPushOpen(symStack *stack); 

symStackItem* stackPosition(symStack *stack, int j);

int reduction(symStack *stack);

void setInFunction(bool in);

/**
 * @brief This function creates compatibility between enums used for data types * 
 * @param prec TokenType variable type
 * @return stVarType variable type
 */
stVarType precTypeToSymtableType (TokenType prec);

#endif

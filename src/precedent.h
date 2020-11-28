/*************************************************************
*  Předmět: IFJ / IAL                                        *
*  Projekt: Implementace compilátoru imperativního jazyka    *
*  Soubor:  precedent.h                                      *
*  Tým: 087                                                  *
*  Varianta: 1                                               *
*  Autoři:  Jan Pospíšil    <xpospi94>                       *
*           Radek Sahliger  <xsahli00>                       *
*           Michal Jireš    <xjires02>                       *
*           Čermák Attila   <xcerma38>                       *
**************************************************************/

#ifndef _PRECEDENT_H
#define _PRECEDENT_H

#include <stdio.h>
#include <stdlib.h>

#include "scanner.h"
#include "symtable.h"
#include "symstack.h"
#include "error.h"
#include "string.h"

int precedent_analys(Token* help);

int idkfunkce(symStack *stack, Token* token);

Token_type StackTopTerm (symStack *stack);

int stackPushOpen(symStack *stack); 

symStackItem* stackPosition(symStack *stack, int j);

int reduction(symStack *stack);

void setGlobalVariables(Token* tok, tSymtable* glob);

void setLocTable(tSymtable* local);

void setInFunction(bool in);

#endif

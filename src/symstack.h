/*************************************************************
*  Předmět: IFJ / IAL                                        *
*  Projekt: Implementace compilátoru imperativního jazyka    *
*  Soubor:  symstack.h                                       *
*  Tým: 087                                                  *
*  Varianta: 1                                               *
*  Autoři:  Jan Pospíšil    <xpospi94>                       *
*           Radek Sahliger  <xsahli00>                       *
*           Michal Jireš    <xjires02>                       *
*           Čermák Attila   <xcerma38>                       *
**************************************************************/

#ifndef _SYMSTACK_H
#define _SYMSTACK_H
#include<stdio.h>
#include<stdlib.h>
#include<stdarg.h>
#include<stdbool.h>

#include "symtable.h"
#include "scanner.h"

//polozka stacku
typedef struct stackItem {
  Token_type tokenType;
  int inte;
  double flt;
  Dynamic_string* string;
  struct stackItem *next;
  char* nazev;
} symStackItem;

//stack
typedef struct {
  symStackItem *top;
} symStack;

//inicializace stacku
void symstackInit(symStack* stack);
//vlozeni symbolu do stacku
bool symstackPush(symStack* stack, Token_type token);
//popnuti stacku jednou
bool symstackPop(symStack* stack);
//popnuti stacku vicekrat
void symstackPopMore(symStack* stack, int howmany);
//vrati ukazatel na item na vrchu stacku
Token_type symstackTop(symStack* stack);
//zrusi stack
void symstackFree(symStack* stack);

#endif

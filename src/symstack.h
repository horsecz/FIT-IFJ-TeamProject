
#ifndef _SYMSTACK_H
#define _SYMSTACK_H
#include<stdio.h>
#include<stdlib.h>
#include<stdarg.h>
#include<stdbool.h>

#include "symtable.h"
#include "scanner.h"
#include "str.h"

//polozka stacku
typedef struct stackItem {
  TokenType token_Type;
  int inte;
  double flt;
  bool boolen;
  string string;
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
bool symstackPush(symStack* stack, TokenType token);
//popnuti stacku jednou
bool symstackPop(symStack* stack);
//popnuti stacku vicekrat
void symstackPopMore(symStack* stack, int howmany);
//vrati ukazatel na item na vrchu stacku
TokenType symstackTop(symStack* stack);
//zrusi stack
void symstackFree(symStack* stack);

#endif

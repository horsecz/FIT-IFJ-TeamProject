/*************************************************************
*  Předmět: IFJ / IAL                                        *
*  Projekt: Implementace compilátoru imperativního jazyka    *
*  Soubor:  symstack.c                                       *
*  Tým: 087                                                  *
*  Varianta: 1                                               *
*  Autoři:  Jan Pospíšil    <xpospi94>                       *
*           Radek Sahliger  <xsahli00>                       *
*           Michal Jireš    <xjires02>                       *
*           Čermák Attila   <xcerma38>                       *
**************************************************************/

#include<stdio.h>
#include<stdlib.h>
#include<stdarg.h>
#include<stdbool.h>

#include "symstack.h"

void symstackInit(symStack* stack){
  stack->top = NULL;
}

bool symstackPush(symStack* stack, Token_type token){
  symStackItem* nitem = (symStackItem*)malloc(sizeof(symStackItem));

  if(nitem != NULL){
    nitem->tokenType = token;
    nitem->inte = 0;
    nitem->flt = 0.0;
    nitem->next = stack->top;
    nitem->nazev = NULL;
    Dynamic_string str;
    d_string_init(&str);
    nitem->string = &str;
    stack->top = nitem;
    return true;
  }else{
    return false;
  }
}

bool symstackPop(symStack* stack){
  if(stack->top != NULL){
    symStackItem* out = stack->top;
    stack->top = out->next;
    free(out);
    
    return true;
  }else{
    return false;
  }
}

void symstackPopMore(symStack* stack, int howmany){
  for(int i = 0; i < howmany; i++){
    symstackPop(stack);
  }
}

Token_type symstackTop(symStack* stack){
  return stack->top->tokenType;
}

void symstackFree(symStack* stack){
  while(symstackPop(stack));
}

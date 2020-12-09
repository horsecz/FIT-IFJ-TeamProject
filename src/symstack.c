/**
 * @file precedent.h
 * @author Jan Pospisil (xpospi94@vutbr.cz)
 * @brief Stack for precedent analysis
 */
#include<stdio.h>
#include<stdlib.h>
#include<stdarg.h>
#include<stdbool.h>

#include "symstack.h"

void symstackInit(symStack* stack){
  stack->top = NULL;
}

bool symstackPush(symStack* stack, TokenType token){
  symStackItem* nitem = (symStackItem*)malloc(sizeof(symStackItem));

  if(nitem != NULL){
    nitem->token_Type = token;
    nitem->inte = 0;
    nitem->flt = 0.0;
    nitem->next = stack->top;
    strInit(&nitem->string);
    nitem->boolen = NULL;
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

TokenType symstackTop(symStack* stack){
  return stack->top->token_Type;
}

void symstackFree(symStack* stack){
  while(symstackPop(stack));
}

/**
 * Project: Implementace překladače imperativního jazyka IFJ20
 *
 * @name ifj2020.c
 * @brief Main body of project performing necessary operations.
 *
 * @authors
 *      Hais Lukas (xhaisl00)
 */
#include <stdio.h>
#include <stdlib.h>
#include "ifj2020.h"

int main()
{
  // create token
  Token *token = (Token*) malloc(sizeof(Token));
  if (token == NULL) {
      printf("Unable to alloc token.");
      return 1; // todo <- set right error code
  }
  token->type = TYPE_EMPTY;
  token->attribute.keyword = KEYWORD_EMPTY;

  int holdResponse = parser(token); //parser(&SymbolTable, &InstructionsList, token);

    // add cleanup of string (fixes a lot of bugs - 80 bytes lost remaining)
  if (token->type == TYPE_IDENTIFIER || token->type == TYPE_STRING)
    strFree(&token->attribute.string);

  free(token);
  //st_destruct(SymbolTable);
  //instructionsList_destruct(InstructionsList);

  return holdResponse;
}

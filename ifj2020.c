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
#include "scanner.c"

int main(int argc, char **argv)
{
  if (argc == 1)
  {
    printf("No parameters passed, program %s is exiting\n", argv[0]);
    return 1;
  }

  // open source file for reading
  FILE *src = fopen(argv[1], "r");
  if (src == NULL)
  {
    printf("Unable to open source file, exiting.\n");
    return 1;
  }
  else setSourceFile(src);

  // create token
  Token *token = (Token*) malloc(sizeof(Token));
  if (token == NULL) {
      printf("Unable to alloc token.");
      return 1; // todo <- set right error code
  }
  token->type = TYPE_EMPTY;
  token->attribute.keyword = KEYWORD_EMPTY;

  int holdResponse;

  while (token->type != TYPE_EOF)
  {
    holdResponse = getToken(token);
    if (holdResponse != 0)
    {
      // close read file & unset source file
      unsetSourceFile();
      fclose(src);

      // free up token string and token itself from memory
      strFree(&token->attribute.string);
      free(token);

      printf("Error in scanner, exiting...\n");
      return holdResponse;
    }

  }

  // close read file & unset source file
  unsetSourceFile();
  fclose(src);

  free(token);

  return 0;
}

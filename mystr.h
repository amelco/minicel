#ifndef MYSTR_H
#define MYSTR_H

#include <string.h>
#include <stdlib.h>
#include "globals.h"

char**
string_split(char* content, const char* sep, int* qtd)
{
  // inicialmente aloca 50 Strings
  // TODO: realocar quando passar de 50
  char** list = malloc(sizeof(char*) * NUM_MAX_ITEMS);

  char* tmp = strtok(content, sep);
  if (tmp == NULL)
  {
    return NULL;
  }

  size_t count = 0;
  while (tmp != NULL)
  {
    list[count] = tmp;
    tmp = strtok(NULL, sep);
    count++;
  }

  if (qtd)
    *qtd = count;

  return list;
}

char* string_trim(char* input)
{
  int i=0;
  while (input[i] == ' ')
  {
    input = &input[i+1];
  }
  i = strlen(input) - 1;
  while (input[i] == ' ')
  {
    input[i] = '\0';
    --i;
  }
  return input;
}


#endif //MYSTR_H

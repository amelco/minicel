#ifndef CELL_H
#define CELL_H

#include <stdlib.h>
#include <stdio.h>

typedef enum
{
  CELLTYPE_TEXT,
  CELLTYPE_NUM,
  CELLTYPE_EXPR
} CellType;

typedef struct
{
  CellType type;
  char* data;
  int row;
  int col;
  int evaluation;
} Cell;

char*
get_cell_value(int row, int col, Cell* cells, int size)
{
  for (int i=0; i<size; ++i)
  {
    if (cells[i].row == row && cells[i].col == col)
      return cells[i].data;
  }
  return NULL;
}

int
evaluate_expression(char* exp)
{
  printf("!! EVALUATE: %s !!\n", exp);
  return 0;
}




#endif //CELL_H


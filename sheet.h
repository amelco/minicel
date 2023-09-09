
#ifndef SHEET_C
#define SHEET_C

#include "cell.h"

typedef struct
{
  Cell* cells;
  int num_rows;
  int num_cols;
} Sheet;

#endif //SHEET_C

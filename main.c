#include <assert.h>
#include <errno.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "globals.h"
#include "mystr.h"
#include "cell.h"
#include "sheet.h"

// Exemplo de uso
// ./minicel in.csv > output.csv


void usage()
{
  fprintf(stderr, "Usage: ./minicel <input.csv>\n");
}

char*
read_content(const char* file_path, size_t* size)
{
  char* buffer = NULL;
  
  FILE *f = fopen(file_path, "rb");  // abre arquivo em modo leitura de bytes
  if (f == NULL)
    goto error;

  if (fseek(f, 0, SEEK_END) < 0)  // aponta o cursor para o fim do arquivo
    goto error;

  long s = ftell(f);  // retorna a posição do cursor (tamanho em bytes já que é no fim do arquivo)
  if (s < 0)
    goto error;

  buffer = malloc(sizeof(char) * s);  // aloca a quantidade de memória necessária para armazenar todo o arquivo
  if (buffer == NULL)
    goto error;

  if (fseek(f, 0, SEEK_SET) < 0 ) // move o cursor para o inicio do arquivo
    goto error;

  size_t n = fread(buffer, sizeof(char), s, f); // faz a leitura byte a byte e armazena no buffer
  assert(n == (size_t)s);   // garante que o tamanho lido no final é igual ao tamanho em bytes do arquivo

  if (ferror(f))  // verifica se ocorreu algum erro no file stream (fread)
    goto error;

  if (size)
    *size = n;

  fclose(f);

  return buffer;

error:
  if (f) 
    fclose(f);
  if (buffer) 
    free(buffer);
  return NULL;
}


int main(int argc, char* argv[])
{
  if (argc < 2)
  {
    usage();
    fprintf(stderr, "EROR: input file not provided\n");
    exit(1);
  }

  const char* input_file_path = argv[1];
  size_t content_size = 0;

  // armazena conteúdo do arquivo na memória
  char* data = read_content(input_file_path, &content_size);
  if (data == NULL)
  {
    fprintf(stderr, "ERROR: could not open file '%s': %s\n", input_file_path, strerror(errno));
    exit(1);
  }
  
  Sheet sheet;
  sheet.cells = malloc(sizeof(Cell) * NUM_MAX_CELLS);

  int num_rows = 0;
  int num_cols = 0;
  char** rows = string_split(data, "\n", &num_rows);
  int k = 0;
  for (int i = 0; i < num_rows; ++i)
  {
    char** cols = string_split(rows[i], "|", &num_cols);
    for (int j=0; j < num_cols; ++j)
    {
      char* value = string_trim(cols[j]);
      Cell cell;

      cell.type = CELLTYPE_TEXT;
      if (value[0] == '=') cell.type = CELLTYPE_EXPR;
      int value_num = strtol(value, NULL, 10);
      if (value_num) cell.type = CELLTYPE_NUM;

      cell.col = j;
      cell.row = i;
      cell.data = value;

      sheet.cells[k] = cell;
      
      k++;
      if (num_cols > sheet.num_cols)
        sheet.num_cols = num_cols;
    }
    if (num_rows > sheet.num_rows)
      sheet.num_rows = num_rows;

    free(cols);
  }

  printf("Table size: %dx%d\n", sheet.num_rows, sheet.num_cols);
  printf("Cell(3,2) = %s\n", get_cell_value(3, 2, sheet.cells, NUM_MAX_CELLS));

  for (int i = 0; i < k; ++i)
  {
    printf("cell(%d, %d): %s => %d\n", sheet.cells[i].row, sheet.cells[i].col, sheet.cells[i].data, sheet.cells[i].type);
    if (sheet.cells[i].type == CELLTYPE_EXPR)
    {
      sheet.cells[i].evaluation = evaluate_expression(sheet.cells[i].data);
    }
  }

  free(sheet.cells);
  free(rows);
  free(data);

  return 0;
}

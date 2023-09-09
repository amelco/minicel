#include <assert.h>
#include <errno.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Exemplo de uso
// ./minicel in.csv > output.csv

#define NUM_MAX_ITEMS 50
#define NUM_MAX_CELLS 500

void usage()
{
  fprintf(stderr, "Usage: ./minicel <input.csv>\n");
}

char*
read_content(const char* file_path, size_t* size)
{
  (void) size;
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

  Cell* cells = malloc(sizeof(Cell) * NUM_MAX_CELLS);

  int num_rows = 0;
  int num_cols = 0;
  int max_rows = 0;
  int max_cols = 0;
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

      cells[k] = cell;
      
      k++;
      if (num_cols > max_cols)
        max_cols = num_cols;
    }
    if (num_rows > max_rows)
      max_rows = num_rows;

    free(cols);
  }

  printf("Table size: %dx%d\n", max_rows, max_cols);
  printf("Cell(3,2) = %s\n", get_cell_value(3, 2, cells, NUM_MAX_CELLS));

  for (int i = 0; i < k; ++i)
  {
    printf("cell(%d, %d): %s => %d\n", cells[i].row, cells[i].col, cells[i].data, cells[i].type);
    if (cells[i].type == CELLTYPE_EXPR)
    {
      cells[i].evaluation = evaluate_expression(cells[i].data);
    }
  }

  free(cells);
  free(rows);
  free(data);

  return 0;
}

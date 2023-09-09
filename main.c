#include <assert.h>
#include <errno.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Exemplo de uso
// ./minicel in.csv > output.csv

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
  char** list = malloc(sizeof(char*) * 50);

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
} Cell;

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

  Cell* c = malloc(sizeof(Cell));

  int qtd;
  char** rows = string_split(data, "\n", &qtd);
  int k = 0;
  for (int i = 0; i < qtd; ++i)
  {
    int num_rows = 0;
    char** cols = string_split(rows[i], "|", &num_rows);
    for (int j=0; j < num_rows; ++j)
    {
      c->type = CELLTYPE_TEXT;
      c->col = j;
      c->row = i;
      c->data = string_trim(cols[j]);
      k++;
      printf("cell(%d, %d): %s\n", c->row, c->col, c->data);
    }
    free(cols);
  }
  free(rows);
  free(c);
  free(data);

  return 0;
}

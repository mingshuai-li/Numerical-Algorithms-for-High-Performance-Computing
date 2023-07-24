#include <stdio.h>
#include <stdlib.h>

#include "include/csr_format.h"

CSRFormat *make_csr_format(const int num_non_zero, const int num_rows)
{
  CSRFormat *csr_format;

  csr_format = malloc(sizeof *csr_format);
  csr_format->num_non_zero = num_non_zero;
  csr_format->num_rows = num_rows;
  csr_format->values = malloc(num_non_zero * sizeof *csr_format->values);
  csr_format->col_indices = malloc(num_non_zero * sizeof *csr_format->col_indices);
  csr_format->non_zero_until_row =
      malloc((num_rows + 1) * sizeof *csr_format->non_zero_until_row);

  return csr_format;
}

void free_csr_format(CSRFormat *csr_format)
{
  free(csr_format->values);
  free(csr_format->col_indices);
  free(csr_format->non_zero_until_row);
  free(csr_format);
}

CSRFormat *get_csr_format(double **matrix, const Dimension matrix_dim,
                          const int num_non_zero)
{
  CSRFormat *csr_format = make_csr_format(num_non_zero, matrix_dim.num_rows);
  int index_non_zero = 0;

  csr_format->non_zero_until_row[0] = 0;
  for (int i = 0; i < matrix_dim.num_rows; i++)
  {
    for (int j = 0; j < matrix_dim.num_columns; j++)
    {
      double value = matrix[i][j];
      if (value != 0)
      {
        csr_format->values[index_non_zero] = value;
        csr_format->col_indices[index_non_zero] = j;
        index_non_zero++;
      }
      csr_format->non_zero_until_row[i + 1] = index_non_zero;
    }
  }

  return csr_format;
}

void multiply_csr_format_with_vector(CSRFormat *csr_format, double *vector,
                                     double *result)
{
  for (int r = 0; r < csr_format->num_rows; r++)
  {
    for (int k = csr_format->non_zero_until_row[r];
         k < csr_format->non_zero_until_row[r + 1]; k++)
    {
      result[r] += csr_format->values[k] * vector[csr_format->col_indices[k]];
    }
  }
}

void print_csr_format(const CSRFormat *csr_format)
{
  printf("| Values\t| Col indices\t| Num non zero\t|\n");
  for (int r = 0; r < csr_format->num_rows; r++)
  {
    for (int i = csr_format->non_zero_until_row[r];
         i < csr_format->non_zero_until_row[r + 1]; i++)
    {
      printf("| %lf\t| %d\t\t| \t\t|\n", csr_format->values[i],
             csr_format->col_indices[i] + 1);
    }
    printf("| \t\t| \t\t| %d\t\t|\n", csr_format->non_zero_until_row[r + 1]);
  }
  printf("\n");
}

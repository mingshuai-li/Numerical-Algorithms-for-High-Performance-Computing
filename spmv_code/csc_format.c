#include <stdio.h>
#include <stdlib.h>

#include "include/csc_format.h"

CSCFormat *make_csc_format(const int num_non_zero, const int num_cols)
{
  CSCFormat *csc_format;

  csc_format = malloc(sizeof *csc_format);
  csc_format->num_non_zero = num_non_zero;
  csc_format->num_cols = num_cols;
  csc_format->values = malloc(num_non_zero * sizeof *csc_format->values);
  csc_format->row_indices = malloc(num_non_zero * sizeof *csc_format->row_indices);
  csc_format->non_zero_until_col =
      malloc((num_cols + 1) * sizeof *csc_format->non_zero_until_col);

  return csc_format;
}

void free_csc_format(CSCFormat *csc_format)
{
  free(csc_format->values);
  free(csc_format->row_indices);
  free(csc_format->non_zero_until_col);
  free(csc_format);
}

CSCFormat *get_csc_format(double **matrix, const Dimension matrix_dim,
                          const int num_non_zero)
{
  CSCFormat *csc_format = make_csc_format(num_non_zero, matrix_dim.num_columns);
  int index_non_zero = 0;

  csc_format->non_zero_until_col[0] = 0;
  for (int j = 0; j < matrix_dim.num_columns; j++)
  {
    for (int i = 0; i < matrix_dim.num_rows; i++)
    {
      double value = matrix[i][j];
      if (value != 0)
      {
        csc_format->values[index_non_zero] = value;
        csc_format->row_indices[index_non_zero] = i;
        index_non_zero++;
      }
      csc_format->non_zero_until_col[j + 1] = index_non_zero;
    }
  }
  return csc_format;
}

void multiply_csc_format_with_vector(CSCFormat *csc_format, double *vector,
                                     double *result)
{
  for (int c = 0; c < csc_format->num_cols; c++)
  {
    for (int k = csc_format->non_zero_until_col[c];
         k < csc_format->non_zero_until_col[c + 1]; k++)
    {
      result[csc_format->row_indices[k]] += csc_format->values[k] * vector[c];
    }
  }
}

void print_csc_format(const CSCFormat *csc_format)
{
  printf("| Values\t| Row indices\t| Num non zero\t|\n");
  for (int c = 0; c < csc_format->num_cols; c++)
  {
    for (int i = csc_format->non_zero_until_col[c];
         i < csc_format->non_zero_until_col[c + 1]; i++)
    {
      printf("| %lf\t| %d\t\t| \t\t|\n", csc_format->values[i],
             csc_format->row_indices[i] + 1);
    }
    printf("| \t\t| \t\t| %d\t\t|\n", csc_format->non_zero_until_col[c + 1]);
  }
  printf("\n");
}
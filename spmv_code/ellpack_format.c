#include <stdio.h>
#include <stdlib.h>

#include "include/ellpack_format.h"

ELLPACKFormat *make_ellpack_format(const int max_non_zero_per_row,
                                   const int num_rows)
{
  ELLPACKFormat *ellpack_format;

  ellpack_format = malloc(sizeof *ellpack_format);
  ellpack_format->max_non_zero_per_row = max_non_zero_per_row;
  ellpack_format->num_rows = num_rows;
  ellpack_format->values = malloc(num_rows * max_non_zero_per_row *
                                  sizeof *ellpack_format->values);
  ellpack_format->col_indices = malloc(num_rows * max_non_zero_per_row *
                                       sizeof *ellpack_format->col_indices);

  return ellpack_format;
}

void free_ellpack_format(ELLPACKFormat *ellpack_format)
{
  free(ellpack_format->values);
  free(ellpack_format->col_indices);
  free(ellpack_format);
}

ELLPACKFormat *get_ellpack_format(double **matrix, const Dimension matrix_dim,
                                  const int max_non_zero_per_row)
{
  ELLPACKFormat *ellpack_format = make_ellpack_format(max_non_zero_per_row,
                                                      matrix_dim.num_rows);
  for (int i = 0; i < matrix_dim.num_rows; i++)
  {
    int write_index = i * max_non_zero_per_row; // current write index for the ELLPACK matrices
    for (int j = 0; j < matrix_dim.num_columns; j++)
    {
      double value = matrix[i][j];
      if (value != 0)
      {
        ellpack_format->values[write_index] = value;
        ellpack_format->col_indices[write_index] = j;
        write_index++;
      }
    }
    for (; write_index < (i + 1) * max_non_zero_per_row; write_index++)
    {
      ellpack_format->col_indices[write_index] = -1; // mark as invalid
    }
  }

  return ellpack_format;
}

void multiply_ellpack_format_with_vector(ELLPACKFormat *ellpack_format,
                                         double *vector, double *result)
{
  for (int i = 0; i < ellpack_format->num_rows; i++)
  {
    int row_index = i * ellpack_format->max_non_zero_per_row;
    for (int k = 0; k < ellpack_format->max_non_zero_per_row; k++)
    {
      int index = row_index + k;
      if (ellpack_format->col_indices[index] == -1)
      {
        break;
      }

      result[i] += ellpack_format->values[index] *
                  vector[ellpack_format->col_indices[index]];
    }
  }

  // for (int k = 0; k < ellpack_format->max_non_zero_per_row; k++)
  // {
  //   for (int i = 0; i < ellpack_format->num_rows; i++)
  //   {
  //     int index = i * ellpack_format->max_non_zero_per_row + k;
  //     if (ellpack_format->col_indices[index] != -1)
  //     {
  //       result[i] += ellpack_format->values[index] *
  //                  vector[ellpack_format->col_indices[index]];
  //     }
  //   }
  // }
}

void print_ellpack_format(const ELLPACKFormat *ellpack_format)
{
  printf("| Values\t| Col indices\t| Matrix row\t|\n");
  for (int i = 0; i < ellpack_format->num_rows; i++)
  {
    printf("| \t\t| \t\t| %d\t\t|\n", i + 1);
    int row_index = i * ellpack_format->max_non_zero_per_row;
    for (int k = 0; k < ellpack_format->max_non_zero_per_row; k++)
    {
      int index = row_index + k;
      if (ellpack_format->col_indices[index] != -1)
      {
        printf("| %lf\t| %d\t\t| \t\t|\n", ellpack_format->values[index],
               ellpack_format->col_indices[index] + 1);
      }
    }
  }
  printf("\n");
}
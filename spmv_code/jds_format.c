#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "include/jds_format.h"

JDSFormat *make_jds_format(const int num_non_zero, const int max_non_zero_per_row)
{
  JDSFormat *jds_format;

  jds_format = malloc(sizeof *jds_format);
  jds_format->num_non_zero = num_non_zero;
  jds_format->max_non_zero_per_row = max_non_zero_per_row;
  jds_format->values = malloc(num_non_zero * sizeof *jds_format->values);
  jds_format->col_indices = malloc(num_non_zero * sizeof *jds_format->col_indices);
  jds_format->start = malloc((max_non_zero_per_row + 1) * sizeof *jds_format->start);
  jds_format->permutation = NULL;
  return jds_format;
}

void free_jds_format(JDSFormat *jds_format)
{
  free(jds_format->values);
  free(jds_format->col_indices);
  free(jds_format->start);
  if (jds_format->permutation != NULL)
  {
    free(jds_format->permutation);
  }
  free(jds_format);
}

int *argsort(int *keys, int num_keys)
{
  int *indices = malloc(num_keys * sizeof *indices);
  for (int i = 1; i < num_keys; i++)
  {
    int tmp_key = keys[i];
    int tmp_idx = indices[i] = i;

    int j = i;
    while (j > 0 && keys[j - 1] < tmp_key)
    {
      keys[j] = keys[j - 1];
      indices[j] = indices[j - 1];
      j--;
    }
    keys[j] = tmp_key;
    indices[j] = tmp_idx;
  }
  return indices;
}

JDSFormat *get_jds_format(double **matrix, const Dimension matrix_dim)
{
  int nnz_total = 0;
  int *nnz_per_row = malloc(matrix_dim.num_rows * sizeof *nnz_per_row);
  for (int i = 0; i < matrix_dim.num_rows; i++)
  {
    int nnz_this_row = 0;
    for (int j = 0; j < matrix_dim.num_columns; j++)
    {
      if (matrix[i][j] != 0)
      {
        nnz_this_row++;
      }
    }
    nnz_per_row[i] = nnz_this_row;
    nnz_total += nnz_this_row;
  }

  // get indices of rows in order of nnz
  int *order = argsort(nnz_per_row, matrix_dim.num_rows);
  JDSFormat *jds_format = make_jds_format(nnz_total, nnz_per_row[0]);
  jds_format->permutation = order;
  free(nnz_per_row);

  int index_non_zero = 0;
  jds_format->start[0] = 0;
  for (int k = 0; k < jds_format->max_non_zero_per_row; k++)
  {
    for (int i = 0; i < matrix_dim.num_rows; i++)
    {
      int nnz_this_row = 0;
      for (int j = 0; j < matrix_dim.num_columns; j++)
      {
        double value = matrix[order[i]][j];
        if (value != 0)
        {
          if (nnz_this_row == k)
          {
            jds_format->values[index_non_zero] = value;
            jds_format->col_indices[index_non_zero] = j;
            index_non_zero++;
          }
          nnz_this_row++;
        }
      }
    }
    jds_format->start[k + 1] = index_non_zero;
  }

  return jds_format;
}

void multiply_jds_format_with_vector(JDSFormat *jds_format,
                                     double *vector, double *result)
{
  for (int k = 0; k < jds_format->max_non_zero_per_row; k++)
  {
    for (int i = 0; i < jds_format->start[k + 1] - jds_format->start[k]; i++)
    {
      result[jds_format->permutation[i]] += jds_format->values[jds_format->start[k] + i] * vector[jds_format->col_indices[jds_format->start[k] + i]];
    }
  }
}

void print_jds_format(const JDSFormat *jds_format)
{
  printf("| Values\t| Col indices\t| Start\t|\n");
  for (int r = 0; r < jds_format->max_non_zero_per_row; r++)
  {
    for (int i = jds_format->start[r];
         i < jds_format->start[r + 1]; i++)
    {
      printf("| %lf\t| %d\t\t| \t\t|\n", jds_format->values[i],
             jds_format->col_indices[i] + 1);
    }
    printf("| \t\t| \t\t| %d\t\t|\n", jds_format->start[r + 1]);
  }
  printf("\n");
}
#pragma once

#include "matrix.h"

typedef struct
{
  double *values;
  int *col_indices;
  int num_rows;
  int max_non_zero_per_row;
} ELLPACKFormat;

ELLPACKFormat *make_ellpack_format(const int max_non_zero_per_row,
                                   const int num_rows);
void free_ellpack_format(ELLPACKFormat *ellpack_format);
ELLPACKFormat *get_ellpack_format(double **matrix, const Dimension matrix_dim,
                                  const int max_non_zero_per_row);
// Result must be a vector filled with 0s
void multiply_ellpack_format_with_vector(ELLPACKFormat *ellpack_format,
                                         double *vector, double *result);
void print_ellpack_format(const ELLPACKFormat *ellpack_format);
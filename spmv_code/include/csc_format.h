#pragma once

#include "matrix.h"

typedef struct
{
  double *values;
  int *row_indices;
  int *non_zero_until_col;
  int num_non_zero;
  int num_cols;
} CSCFormat;

CSCFormat *make_csc_format(const int num_non_zero, const int num_cols);
void free_csc_format(CSCFormat *csc_format);
CSCFormat *get_csc_format(double **matrix, const Dimension matrix_dim,
                          const int num_non_zero);
// Result must be a vector filled with 0s
void multiply_csc_format_with_vector(CSCFormat *csc_format, double *vector,
                                     double *result);
void print_csc_format(const CSCFormat *csc_format);    
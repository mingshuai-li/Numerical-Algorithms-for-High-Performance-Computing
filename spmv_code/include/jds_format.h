#pragma once

#include "matrix.h"

typedef struct
{
  double *values;
  int *col_indices;
  int *start;
  int max_non_zero_per_row;
  int num_non_zero;
  int *permutation;
} JDSFormat;

JDSFormat *make_jds_format(const int num_non_zero, const int max_non_zero_per_row);
void free_jds_format(JDSFormat *jds_format);
JDSFormat *get_jds_format(double **matrix, const Dimension matrix_dim);
// Result must be a vector filled with 0s
void multiply_jds_format_with_vector(JDSFormat *jds_format,
                                         double *vector, double *result);
void print_jds_format(const JDSFormat *jds_format);
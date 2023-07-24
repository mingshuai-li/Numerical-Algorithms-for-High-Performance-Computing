#pragma once

#include "buffer.h"

typedef BufferType MatrixType;

typedef struct
{
  int num_rows;
  int num_columns;
} Dimension;

/* Allocates memory for a matrix. The memory is contiguous. */
double **allocate_matrix(const Dimension dimension);

/* Frees the memory allocated for the matrix */
void free_matrix(double **matrix);

void fill_matrix(double **matrix, const Dimension dimension,
                 const MatrixType type);

void print_matrix(double **matrix, const Dimension dimension);

/* The matrix result must be a pointer to a contiguous allocated block memory.
 * This method DOES NOT initialize result to 0s
 */
void multiply_matrices(double **matrix_A, const Dimension dimension_A,
                       double **matrix_B, const Dimension dimension_B,
                       double **result);
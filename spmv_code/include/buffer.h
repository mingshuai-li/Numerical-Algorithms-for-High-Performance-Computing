#pragma once

/*
 * We can have the following types of buffers:
 *  - RANDOM: Completely random values between 0 and 1. Uses default seed
 *  - INDEXED: Increases with the index
 *  - ZEROES: All zeros
 *  - IDENTITY: Matrix[k][k] = 1, Matrix[i][j] = 0 for i != j (only for matrix)
 *  - SPARSE: Sparse matrix with random values. Approx. 30% of the elements
 *  are non-zero, and 1/3 of them will be on the diagonal.
 */
typedef enum
{
  RANDOM,
  INDEXED,
  ZEROES,
  IDENTITY,
  SPARSE
} BufferType;
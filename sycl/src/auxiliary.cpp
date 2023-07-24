#include "auxiliary.h"
#include <algorithm>
#include <boost/progress.hpp>
#include <cstdlib>
#include <iostream>
#include <vector>

// A single matrix element
struct MatrixElement {
  int i{};
  int j{};
  float value{};
};

void convertCooToCsr(CsrMatrix &matrix, const std::vector<int> &I) {
  std::vector<MatrixElement> matrixElements(matrix.nnz);

  // use additional structure for sorting
  for (int x = 0; x < matrix.nnz; ++x) {
    matrixElements[x].i = I[x];
    matrixElements[x].j = matrix.indices[x];
    matrixElements[x].value = matrix.values[x];
  }

  // sort after collumn and then after i
  // std::qsort(m, nnz, sizeof(mat_elem), cmp_by_ij);
  std::sort(matrixElements.begin(), matrixElements.end(),
            [](MatrixElement &first, MatrixElement &second) -> bool {
              if (first.i > second.i) {
                return false;
              } else if (first.i < second.i) {
                return true;
              } else {
                if (first.j > second.j) {
                  return false;
                } else if (first.j < second.j) {
                  return true;
                } else {
                  return false;
                }
              }
            });

  // set all ptr to 0
  for (int x = 0; x < (matrix.numRows + 1); ++x) {
    matrix.start[x] = -1;
  }

  // fill in ptr
  int currRow = 0;
  matrix.start[currRow] = 0;
  for (int x = 0; x < matrix.nnz; ++x) {
    if (matrixElements[x].i > currRow) {
      currRow = matrixElements[x].i;
      matrix.start[currRow] = x;
    }

    matrix.indices[x] = matrixElements[x].j;
    matrix.values[x] = matrixElements[x].value;
  }

  // last element in ptr is set to nnz
  matrix.start[matrix.numRows] = matrix.nnz;

  // all missing rows get the ptr of the previous row
  for (int x = (matrix.numRows - 1); x >= 0; --x) {
    if (matrix.start[x] == -1) {
      matrix.start[x] = matrix.start[x + 1];
    }
  }
}

EllMatrix convertCrsToEllpack(CsrMatrix &matrix) {
  EllMatrix ellMatrix;

  int maxColumns{0};
  for (size_t row = 0; row < matrix.numRows; ++row) {
    int rowStart = matrix.start[row];
    int rowEnd = matrix.start[row + 1];
    int numColumns = rowEnd - rowStart;
    if (numColumns > maxColumns) {
      maxColumns = numColumns;
    }
  }
  std::cout << "max num columns: " << maxColumns << std::endl;
  ellMatrix.numRows = matrix.numRows;
  ellMatrix.numColsPerRow = maxColumns;

  const size_t ellpackSize = ellMatrix.numColsPerRow * ellMatrix.numRows;
  ellMatrix.values.resize(ellpackSize);
  ellMatrix.indices.resize(ellpackSize);

  std::fill_n(ellMatrix.values.begin(), ellpackSize, 0.0);
  std::fill_n(ellMatrix.indices.begin(), ellpackSize, 0);

  for (size_t row = 0; row < matrix.numRows; ++row) {
    int rowStart = matrix.start[row];
    int rowEnd = matrix.start[row + 1];

    for (int j = rowStart, c = 0; j < rowEnd; ++j, ++c) {
      ellMatrix.values[row + c * matrix.numRows] = matrix.values[j];
      ellMatrix.indices[row + c * matrix.numRows] = matrix.indices[j];
    }
  }

  return ellMatrix;
}

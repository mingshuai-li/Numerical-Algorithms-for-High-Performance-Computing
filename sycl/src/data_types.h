#pragma once

#include <limits>
#include <string>
#include <vector>

enum ExecutionMode { CRS, ELLPACK };

struct Configuration {
  ExecutionMode executionMode{ExecutionMode::CRS};
  std::string matrixFile{""};
  bool toPrint{false};
};

struct CsrMatrix {
  int numRows{};
  int nnz{};
  std::vector<int> start{};
  std::vector<int> indices{};
  std::vector<float> values{};
};

struct DevCsrMatrix {
  int numRows{};
  int nnz{};
  int *start{nullptr};
  int *indices{nullptr};
  float *values{nullptr};
};

struct EllMatrix {
  EllMatrix() = default;
  explicit EllMatrix(size_t numRows, int colsPerRow)
      : numRows(numRows), numColsPerRow(colsPerRow) {
    indices.resize(numColsPerRow * numRows, 0);
    values.resize(numColsPerRow * numRows, 0.0f);
  }

  EllMatrix(const EllMatrix &other) = default;
  EllMatrix &operator=(const EllMatrix &other) = default;

  int numColsPerRow{0};
  size_t numRows{0};
  std::vector<int> indices{};
  std::vector<float> values{};
};

struct DevEllMatrix {
  int numColsPerRow{0};
  size_t numRows{0};
  int *indices{nullptr};
  float *values{nullptr};
};

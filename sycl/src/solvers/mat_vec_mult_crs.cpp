#include "CL/sycl.hpp"
#include "solvers/kernels.h"
#include <chrono>

namespace kernel {
void matVecMult(cl::sycl::queue &queue, CsrMatrix &matrix,
                std::vector<float> &x, std::vector<float> &y) {
  auto start = std::chrono::steady_clock::now();
  assert(matrix.numRows == x.size());
  assert(x.size() == y.size());

  DevCsrMatrix devMatrix;
  devMatrix.numRows = matrix.numRows;
  devMatrix.nnz = matrix.nnz;

  // allocated memory on the device
  devMatrix.start = cl::sycl::malloc_shared<int>(matrix.start.size(), queue);
  devMatrix.indices =
      cl::sycl::malloc_shared<int>(matrix.indices.size(), queue);
  devMatrix.values =
      cl::sycl::malloc_shared<float>(matrix.values.size(), queue);
  auto *devX = cl::sycl::malloc_shared<float>(x.size(), queue);
  auto *devY = cl::sycl::malloc_shared<float>(y.size(), queue);

  // copy data to the device
  queue.memcpy(devMatrix.start, matrix.start.data(),
               matrix.start.size() * sizeof(int));
  queue.memcpy(devMatrix.indices, matrix.indices.data(),
               matrix.indices.size() * sizeof(int));
  queue.memcpy(devMatrix.values, matrix.values.data(),
               matrix.values.size() * sizeof(float));
  queue.memcpy(devX, x.data(), x.size() * sizeof(float));
  queue.memcpy(devY, y.data(), y.size() * sizeof(float));
  queue.wait();

  auto startKernel = std::chrono::steady_clock::now();
  cl::sycl::nd_range rng{{static_cast<size_t>(matrix.numRows)}, {1}};
  queue
      .submit([&](cl::sycl::handler &cgh) {
        cgh.parallel_for(rng, [=](cl::sycl::nd_item<1> item) {
          auto row = item.get_global_id();

          float dot = 0.0f;
          int rowStart = devMatrix.start[row];
          int rowEnd = devMatrix.start[row + 1];

          for (int j = rowStart; j < rowEnd; ++j) {
            dot += devMatrix.values[j] * devX[devMatrix.indices[j]];
          }

          devY[row] = dot;
        });
      })
      .wait_and_throw();
  auto endKernel = std::chrono::steady_clock::now();

  // copy results back to the host
  queue.memcpy(const_cast<float *>(y.data()), devY, y.size() * sizeof(float))
      .wait();

  // deallocate device memory
  cl::sycl::free(devMatrix.start, queue);
  cl::sycl::free(devMatrix.indices, queue);
  cl::sycl::free(devMatrix.values, queue);
  cl::sycl::free(devX, queue);
  cl::sycl::free(devY, queue);

  auto end = std::chrono::steady_clock::now();

  std::chrono::duration<double> elapsedKernelSeconds = endKernel - startKernel;
  std::cout << "kernel elapsed time: " << elapsedKernelSeconds.count() << "s\n";

  std::chrono::duration<double> elapsedSeconds = end - start;
  std::cout << "elapsed time: " << elapsedSeconds.count() << "s\n";
}
} // namespace kernel
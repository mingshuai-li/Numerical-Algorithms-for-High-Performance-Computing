#include "solvers/kernels.h"

namespace kernel {
void matVecMult(cl::sycl::queue &queue, EllMatrix &matrix,
                std::vector<float> &x, std::vector<float> &y) {
  auto start = std::chrono::steady_clock::now();

  assert(matrix.numRows == x.size());
  assert(x.size() == y.size());

  DevEllMatrix devMatrix;
  devMatrix.numRows = matrix.numRows;
  devMatrix.numColsPerRow = matrix.numColsPerRow;

  // allocated memory on the device
  devMatrix.indices =
      cl::sycl::malloc_shared<int>(matrix.indices.size(), queue);
  devMatrix.values =
      cl::sycl::malloc_shared<float>(matrix.values.size(), queue);
  auto *devX = cl::sycl::malloc_shared<float>(x.size(), queue);
  auto *devY = cl::sycl::malloc_shared<float>(y.size(), queue);

  // copy data to the device
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

          for (int i = 0; i < devMatrix.numColsPerRow; ++i) {
            int column = devMatrix.indices[row + i * devMatrix.numRows];
            float value = devMatrix.values[row + i * devMatrix.numRows];
            if (value != 0.0f) {
              dot += value * devX[column];
            }
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
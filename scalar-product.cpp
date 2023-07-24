#include <CL/sycl.hpp>
#include <cmath>
#include <iostream>


int main() {
  cl::sycl::default_selector device_selector;
  cl::sycl::queue queue(device_selector);

  // allocate and init array A
  constexpr size_t N{1024};
  auto* A = cl::sycl::malloc_shared<float>(N, queue);
  auto* B = cl::sycl::malloc_shared<float>(N, queue);
  for (size_t i{0}; i < N; ++i) {
    A[i] = 0.25; 
    B[i] = 4.00; 
  }

  auto* C = cl::sycl::malloc_shared<float>(N, queue);
  queue.submit([&] (cl::sycl::handler& cgh) {
    cgh.parallel_for(cl::sycl::range<1>(N),
                     [=](cl::sycl::id<1> i) {
       C[i] = A[i] * B[i];
    });
  }).wait();

  // compute the height of a binary
  const size_t K = static_cast<size_t>(std::log2(N));
  for (size_t k{0}; k < K; ++k) {
    
    // perform a redution operation at the current tree level
    // Note: we are using memory coalesced friendly algorithm
    // (see: https://developer.nvidia.com/blog/how-access-global-memory-efficiently-cuda-c-kernels)
    const size_t numProc = static_cast<size_t>(std::pow(2., K - k - 1));
    queue.submit([&] (cl::sycl::handler& cgh) {
      cgh.parallel_for(cl::sycl::range<1>(numProc),
                       [=](cl::sycl::id<1> i) {
         C[i] = C[i] + C[i + numProc];
      });
    });
  }
   
  // show computed results
  queue.wait_and_throw();
  std::cout << "computed results: " << C[0] << std::endl;

  // show expected results i.e., a sum of the given arithmetic sequence
  std::cout << "expected: " << N << std::endl;

  // free allocated memory
  cl::sycl::free(A, queue);
  cl::sycl::free(B, queue);

  return 0;
}


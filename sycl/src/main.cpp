#include "auxiliary.h"
#include "data_types.h"
#include "solvers/kernels.h"
#include "util.h"
#include <CL/sycl.hpp>
#include <iostream>

int main(int argc, char *argv[]) {
  try {
    Configuration config = makeConfig(argc, argv);
    checkConfig(config);

    cl::sycl::queue queue{cl::sycl::default_selector{}};

    std::cout << "================= SPARSE LINEAR ALGEBRA CONFIGURATION ==================\n";
    std::cout << config << '\n';
    std::cout << "using: "
              << queue.get_device().get_info<cl::sycl::info::device::name>()
              << '\n';
    std::cout << "========================================================================"
              << std::endl;

    CsrMatrix csrMatrix = loadMarketMatrix(config.matrixFile);
    std::vector<float> x(csrMatrix.numRows, 1.0f / static_cast<float>(csrMatrix.numRows));
    std::vector<float> y(csrMatrix.numRows, 0.0f);

    if (config.executionMode == ExecutionMode::CRS) {
      kernel::matVecMult(queue, csrMatrix, x, y);
    } else {
      EllMatrix ellMatrix = convertCrsToEllpack(csrMatrix);
      kernel::matVecMult(queue, ellMatrix, x, y);
    }

    if (config.toPrint) {
      std::cout << "solution: " << std::endl;
      for (size_t i = 0; i < csrMatrix.numRows; ++i) {
        std::cout << i << ")\t" << y[i] << std::endl;
      }
    }

  } catch (std::exception &error) {
    std::cout << "Error: " << error.what() << std::endl;
    exit(EXIT_FAILURE);
  }
  return 0;
}

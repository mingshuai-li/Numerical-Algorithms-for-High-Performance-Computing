#pragma once

#include "CL/sycl.hpp"
#include "data_types.h"

namespace kernel {
void matVecMult(cl::sycl::queue &queue, CsrMatrix &matrix,
                std::vector<float> &x, std::vector<float> &y);
void matVecMult(cl::sycl::queue &queue, EllMatrix &matrix,
                std::vector<float> &x, std::vector<float> &y);
} // namespace kernel
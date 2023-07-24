#pragma once

#include "data_types.h"

void convertCooToCsr(CsrMatrix &matrix, const std::vector<int> &I);
EllMatrix convertCrsToEllpack(CsrMatrix &matrix);

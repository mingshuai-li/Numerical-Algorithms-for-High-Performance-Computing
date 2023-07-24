#pragma once

#include "data_types.h"
#include <iostream>
#include <string>

std::string modeToStr(ExecutionMode mode);
Configuration makeConfig(int argc, char **argv);
void checkConfig(const Configuration &config);
std::ostream &operator<<(std::ostream &stream, const Configuration &config);
CsrMatrix loadMarketMatrix(const std::string &fileName);

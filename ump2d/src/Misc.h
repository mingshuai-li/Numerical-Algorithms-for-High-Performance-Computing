#pragma once

#include "DataTypes.h"
#include <boost/program_options/variables_map.hpp>
#include <vtkUnstructuredGrid.h>
#include <vtkSmartPointer.h>
#include <string>

namespace ump2d::misc {
struct Config {
  std::string inputFileName{};
  std::string outputFileName{};
  size_t numPartitions{4};
  bool verbose{false};
};

Config processCmd(boost::program_options::variables_map&);
vtkSmartPointer<vtkUnstructuredGrid> readMesh(const std::string& inputFilePath);
void checkMesh(vtkUnstructuredGrid* vtkMesh);
void writeMesh(vtkUnstructuredGrid* vtkMesh,
               std::vector<idx_t>& partitions,
               const std::string& outputFileName);
} // namespace ump2d::misc

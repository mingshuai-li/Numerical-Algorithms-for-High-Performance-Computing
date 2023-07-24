#include "Misc.h"
#include <vtkUnstructuredGridReader.h>
#include <vtkUnstructuredGridWriter.h>
#include <vtkUnstructuredGrid.h>
#include <vtkCellData.h>
#include <filesystem>
#include <iostream>

namespace po = boost::program_options;

namespace ump2d::misc {

Config processCmd(boost::program_options::variables_map& vm) {
  Config fileNames{};

  if (vm.count("input")) {
    fileNames.inputFileName = vm["input"].as<std::string>();
    std::filesystem::path path(fileNames.inputFileName);
    if (not std::filesystem::exists(path)) {
      throw std::runtime_error("input file does not exist");
    }
  } else {
    throw std::runtime_error("the input vtk file was not provided");
  }

  if (vm.count("output")) {
    fileNames.outputFileName = vm["output"].as<std::string>();
    std::filesystem::path path(fileNames.outputFileName);
    auto parentDir = path.parent_path();
    if (not std::filesystem::exists(parentDir)) {
      std::filesystem::create_directories(parentDir);
    }
    if (path.extension() != std::string(".vtk")) {
      fileNames.outputFileName += ".vtk";
    }
  } else {
    throw std::runtime_error("the output file was not provided");
  }

  if (vm.count("partitions")) {
    auto partitions = vm["partitions"].as<int>();
    if (partitions > 0) {
      fileNames.numPartitions = static_cast<size_t>(partitions);
    } else {
      throw std::runtime_error("wrong partition number");
    }
  } else {
    throw std::runtime_error("the partition number is not provided");
  }

  if (vm.count("verbose")) {
    fileNames.verbose = true;
  }

  return fileNames;
}

vtkSmartPointer<vtkUnstructuredGrid> readMesh(const std::string& inputFilePath) {
  auto reader = vtkSmartPointer<vtkUnstructuredGridReader>::New();
  reader->SetFileName(inputFilePath.c_str());
  reader->Update();

  auto vtkMesh = vtkSmartPointer<vtkUnstructuredGrid>::New();
  vtkMesh->DeepCopy(reader->GetOutput());
  return std::move(vtkMesh);
}

void checkMesh(vtkUnstructuredGrid* vtkMesh) {
  const auto cellNumber = vtkMesh->GetNumberOfCells();

  std::vector<vtkIdType> notTriangleElements{};
  bool isOk{true};
  for (vtkIdType id = 0; id < cellNumber; ++id) {
    const auto type = vtkMesh->GetCellType(id);
    if (type != VTK_TRIANGLE) {
      isOk = false;
      notTriangleElements.push_back(id);
    }
  }

  if (not isOk) {
    std::stringstream error;
    error << "found not triangular elements: ";
    for (auto id : notTriangleElements) {
      error << std::to_string(id) << ", ";
    }
    throw std::runtime_error(error.str());
  }
}

void writeMesh(vtkUnstructuredGrid* vtkMesh,
               std::vector<idx_t>& partitions,
               const std::string& outputFileName) {

  vtkNew<vtkIntArray> partitioningData;
  partitioningData->SetNumberOfComponents(1);
  partitioningData->SetName("partitions");
  for (auto part : partitions) {
    partitioningData->InsertNextValue(static_cast<int>(part));
  }
  vtkMesh->GetCellData()->AddArray(partitioningData);

  auto writer = vtkSmartPointer<vtkUnstructuredGridWriter>::New();
  writer->SetFileName(outputFileName.c_str());
  writer->SetInputData(vtkMesh);
  writer->Write();
}
} // namespace ump2d::misc

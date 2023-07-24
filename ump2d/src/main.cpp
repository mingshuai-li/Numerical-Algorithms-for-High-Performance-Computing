#include "Misc.h"
#include "GraphUtils.h"
#include <boost/program_options/options_description.hpp>
#include <boost/program_options/parsers.hpp>
#include <boost/program_options/variables_map.hpp>
#include <iostream>

namespace po = boost::program_options;

int main(int argc, char* argv[]) {
  po::options_description desc{"Options"};
  // clang-format off
  desc.add_options() ("help,h", "Print help message")
                     ("input,i", po::value<std::string>(), "input vtk file")
                     ("output,o", po::value<std::string>(), "output file")
                     ("partitions,p", po::value<int>(), "num. partitions")
                     ("verbose,v", "print metis info during partitioning");
  // clang-format on

  po::variables_map vm;
  po::store(po::parse_command_line(argc, argv, desc), vm);
  po::notify(vm);

  if (vm.count("help")) {
    std::cout << desc << std::endl;
    return 0;
  }

  ump2d::misc::Config config{};
  vtkSmartPointer<vtkUnstructuredGrid> vtkMesh;
  try {
    config = ump2d::misc::processCmd(vm);
    vtkMesh = ump2d::misc::readMesh(config.inputFileName);
    ump2d::misc::checkMesh(vtkMesh.Get());
  } catch (std::runtime_error& err) {
    std::cerr << "error: " << err.what() << std::endl;
    return -1;
  }

  auto ellpackGraph = ump2d::graph_utils::build(vtkMesh.Get());
  auto crsGraph = ump2d::graph_utils::convertToMetisFormat(ellpackGraph);
  auto partitions =
      ump2d::graph_utils::performPartitioning(crsGraph, config.numPartitions, config.verbose);

  if (partitions.has_value()) {
    ump2d::misc::writeMesh(vtkMesh, partitions.value(), config.outputFileName);
  } else {
    return -1;
  }

  return 0;
}

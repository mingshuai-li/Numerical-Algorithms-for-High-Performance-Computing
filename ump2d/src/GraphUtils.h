#pragma once

#include "DataTypes.h"
#include <vtkUnstructuredGrid.h>
#include <optional>

namespace ump2d::graph_utils {
graph::EllPack build(vtkUnstructuredGrid* mesh);
graph::Crs convertToMetisFormat(graph::EllPack& graph);

std::optional<std::vector<idx_t>>
    performPartitioning(graph::Crs& graph, size_t numPartitions, bool verbose);
} // namespace ump2d::graph_utils
#include "GraphUtils.h"
#include <cassert>
#include <iostream>

namespace ump2d::graph_utils {
graph::EllPack build(vtkUnstructuredGrid* mesh) {

  assert(mesh != nullptr);
  const auto cellNumber = mesh->GetNumberOfCells();

  constexpr int maxNumNeighbours{3};
  graph::EllPack graph(cellNumber, maxNumNeighbours);
  for (vtkIdType cid = 0; cid < cellNumber; ++cid) {
    auto* cell = mesh->GetCell(cid);
    auto* points = cell->GetPointIds();

    for (long long side = 0; side < maxNumNeighbours; ++side) {
      vtkNew<vtkIdList> edge;
      edge->InsertNextId(points->GetId(side));
      edge->InsertNextId(points->GetId((side + 1) % 3));

      vtkNew<vtkIdList> edgeNeighbours;
      mesh->GetCellNeighbors(cid, edge, edgeNeighbours);
      for (vtkIdType nid = 0; nid < edgeNeighbours->GetNumberOfIds(); ++nid) {
        graph[cid][side] = edgeNeighbours->GetId(nid);
      }
    }
  }
  return graph;
}

graph::Crs convertToMetisFormat(graph::EllPack& ellPackGraph) {

  const auto numVertices = ellPackGraph.getNumVertices();
  const auto totalNumEdges = ellPackGraph.getTotalNumEdges();
  graph::Crs crsGraph(numVertices, totalNumEdges);

  crsGraph.xadj(0) = 0;
  size_t globalEdgeCounter{0};
  for (size_t vertexId = 0; vertexId < numVertices; ++vertexId) {

    const size_t currNumEdges = crsGraph.xadj(vertexId);
    size_t localEdgeCounter{0};
    for (size_t nid = 0; nid < 3; ++nid) {
      const size_t neighbourVertex = ellPackGraph[vertexId][nid];
      if (neighbourVertex != graph::EllPack::nan) {
        crsGraph.adjncy(currNumEdges + localEdgeCounter) = static_cast<idx_t>(neighbourVertex);
        ++localEdgeCounter;
      }
    }

    globalEdgeCounter += localEdgeCounter;
    crsGraph.xadj(vertexId + 1) = static_cast<idx_t>(globalEdgeCounter);
  }
  return crsGraph;
}

std::optional<std::vector<idx_t>>
    performPartitioning(graph::Crs& graph, size_t numPartitions, bool verbose) {
  idx_t numVertices = graph.getNumVertices();
  std::vector<idx_t> partitions(numVertices);
  idx_t numConstraints{1};
  idx_t edgeCut{0};

  idx_t options[METIS_NOPTIONS];
  METIS_SetDefaultOptions(options);

  if (verbose) {
    options[METIS_OPTION_DBGLVL] = 0b1110001111;
  }

  auto result = METIS_PartGraphKway(&numVertices,                             // idx_t *nvtxs,
                                    &numConstraints,                          // idx_t *ncon,
                                    graph.getXadjData(),                      // idx_t *xadj,
                                    graph.getAdjncyData(),                    // idx_t *adjncy,
                                    nullptr,                                  // idx_t *vwgt,
                                    nullptr,                                  // idx_t *vsize,
                                    nullptr,                                  // idx_t *adjwgt,
                                    reinterpret_cast<idx_t*>(&numPartitions), // idx_t *nparts,
                                    nullptr,                                  // real_t *tpwgts,
                                    nullptr,                                  // real_t *ubvec,
                                    options,                                  // idx_t *options,
                                    &edgeCut,                                 // idx_t *edgecut,
                                    const_cast<idx_t*>(partitions.data())     // idx_t *part
  );

  if (result != METIS_OK) {
    std::cerr << "partitioning failed" << std::endl;
    return std::optional<std::vector<idx_t>>{};
  }

  std::cout << "total edge cut: " << edgeCut << std::endl;
  return std::optional<std::vector<idx_t>>{partitions};
}
} // namespace ump2d::graph_utils
#pragma once

#include "metis.h"
#include <cstddef>
#include <limits>
#include <vector>
#include <numeric>

namespace ump2d::graph {
struct EllPack {
  static inline constexpr size_t nan = std::numeric_limits<size_t>::max();

  EllPack(size_t numVertices, size_t maxNumNeighbours) : maxNumNeighbours(maxNumNeighbours) {
    adjncy.resize(numVertices * maxNumNeighbours);
    std::fill(adjncy.begin(), adjncy.end(), nan);
  }

  size_t* operator[](size_t vertexId) { return &adjncy[vertexId * maxNumNeighbours]; }
  size_t getNumVertices() { return adjncy.size() / maxNumNeighbours; }

  size_t getTotalNumEdges() {
    auto filter = [](size_t& accumulator, const size_t& nid) {
      const size_t value = (nid == nan) ? 0 : 1;
      return accumulator + value;
    };

    return std::accumulate(adjncy.begin(), adjncy.end(), static_cast<size_t>(0), filter);
  }

  private:
  std::vector<size_t> adjncy{};
  size_t maxNumNeighbours{};
};

struct Crs {
  explicit Crs(size_t numVertices, size_t numDirectEdges) {
    xadjData.resize(numVertices + 1);
    adjncyData.resize(numDirectEdges);
  }

  idx_t& xadj(size_t vertexId) { return xadjData[vertexId]; }
  idx_t* getXadjData() { return xadjData.data(); }
  idx_t& adjncy(size_t i) { return adjncyData[i]; }
  idx_t* getAdjncyData() { return adjncyData.data(); }
  idx_t getNumVertices() { return static_cast<idx_t>(xadjData.size() - 1); }

  private:
  std::vector<idx_t> xadjData{};
  std::vector<idx_t> adjncyData{};
};
} // namespace ump2d::graph
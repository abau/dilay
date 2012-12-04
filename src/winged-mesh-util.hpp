#include "winged-mesh.hpp"
#include "winged-edge.hpp"
#include "maybe.hpp"

#ifndef WINGED_MESH_UTIL
#define WINGED_MESH_UTIL

namespace WingedMeshUtil {
  void                  printStatistics (const WingedMesh&);
  MaybePtr <LinkedEdge> findEdge        (WingedMesh&, unsigned int, unsigned int);
}

#endif

#ifndef WINGED_MESH_UTIL
#define WINGED_MESH_UTIL

#include "winged-mesh.hpp"

namespace WingedMeshUtil {
  void printStatistics (const WingedMesh&,bool = false);
  void fromMesh        (WingedMesh&, const Mesh&);
}

#endif

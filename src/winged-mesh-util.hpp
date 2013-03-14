#ifndef DILAY_WINGED_MESH_UTIL
#define DILAY_WINGED_MESH_UTIL

#include "fwd-winged.hpp"

class Mesh;

namespace WingedMeshUtil {
  void printStatistics (const WingedMesh&, bool = false);
  void fromMesh        (WingedMesh&, const Mesh&);
}

#endif

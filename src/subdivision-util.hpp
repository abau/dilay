#include "winged-mesh.hpp"

#ifndef SUBDIVISION_UTIL
#define SUBDIVISION_UTIL

namespace SubdivUtil {
  void splitEdge         (WingedMesh&, LinkedEdge&);
  void splitEdge         (WingedMesh&, LinkedEdge&, const glm::vec3&);
  void splitFaceRegular  (WingedMesh&, LinkedFace&);
  void equalizeFaces     (WingedMesh&, LinkedFace&, LinkedEdge&);
}

#endif

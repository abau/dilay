#include "winged-mesh.hpp"

#ifndef SUBDIVISION_UTIL
#define SUBDIVISION_UTIL

typedef std::pair <LinkedFace*,LinkedFace*> NewFaces;

namespace SubdivUtil {
  NewFaces      splitEdge    (WingedMesh&, LinkedEdge&);
  NewFaces      splitEdge    (WingedMesh&, LinkedEdge&, const glm::vec3&);
  LinkedEdge*   insertVertex (WingedMesh&, LinkedEdge&, const glm::vec3&);
}

#endif

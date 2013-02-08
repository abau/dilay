#include "winged-mesh.hpp"

#ifndef ADAPTIVE_MESH
#define ADAPTIVE_MESH

namespace AdaptiveMesh {
  void splitEdge         (WingedMesh&, LinkedEdge&);
  void splitFace         (WingedMesh&, LinkedFace&);
  void splitFaceRegular  (WingedMesh&, LinkedFace&);
  void equalizeFaces     (WingedMesh&, LinkedFace&, LinkedEdge&);
}

#endif

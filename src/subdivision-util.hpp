#include "winged-mesh.hpp"

#ifndef SUBDIVISION_UTIL
#define SUBDIVISION_UTIL

typedef std::pair <LinkedFace,LinkedFace> NewFaces;

namespace SubdivUtil {
  /*
  NewFaces      splitEdge    (WingedMesh&, LinkedEdge);
  NewFaces      splitEdge    (WingedMesh&, LinkedEdge, const glm::vec3&);
  */
  LinkedEdge    insertVertex (WingedMesh&, LinkedEdge, const glm::vec3&);

  /** `triangluate6Gon(m,f)` triangulates the 6-gon `f`.
   * Note thate `f->edge ()->firstVertex (*f)` must be an even vertex, i.e.
   * it must not be generated during the current subdivision step.
   */
  void          triangulate6Gon (WingedMesh&, LinkedFace);
}

#endif

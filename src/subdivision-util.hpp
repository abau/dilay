
#ifndef DILAY_SUBDIVISION_UTIL
#define DILAY_SUBDIVISION_UTIL

#include "fwd-glm.hpp"
#include "fwd-winged.hpp"

namespace SubdivUtil {
  LinkedEdge    insertVertex (WingedMesh&, LinkedEdge, const glm::vec3&);

  /** `triangluate6Gon(m,f)` triangulates the 6-gon `f`.
   * Note thate `f->edge ()->firstVertex (*f)` must be an even vertex, i.e.
   * it must not be generated during the current subdivision step.
   */
  void          triangulate6Gon (WingedMesh&, LinkedFace);
}

#endif

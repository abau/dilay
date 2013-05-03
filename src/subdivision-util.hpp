#ifndef DILAY_SUBDIVISION_UTIL
#define DILAY_SUBDIVISION_UTIL

#include "fwd-glm.hpp"
#include "fwd-winged.hpp"

namespace SubdivUtil {
  /** `insertVertex (m,e,v)` inserts a new vertex `v` at edge `e`.
   * The new edge (from the first vertex of `e` to `v`) is returned.
   */
  LinkedEdge insertVertex (WingedMesh&, LinkedEdge, const glm::vec3&);

  /** `triangulate6Gon(m,f)` triangulates the 6-gon `f`.
   * Note thate `f->edge ()->firstVertex (*f)` must be an even vertex, i.e.
   * it must not be generated during the current subdivision step.
   * The new faces are adjacent to `f`.
   */
  void triangulate6Gon (WingedMesh&, LinkedFace);

  /** `triangulateQuadAtHeighestLevelVertex (m,f)` triangulates the quad `f` by 
   * inserting an edge from the vertex of `f` with the heighest level. */
  void triangulateQuadAtHeighestLevelVertex (WingedMesh&, LinkedFace);
}

#endif

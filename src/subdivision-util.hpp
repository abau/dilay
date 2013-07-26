#ifndef DILAY_SUBDIVISION_UTIL
#define DILAY_SUBDIVISION_UTIL

#include "fwd-glm.hpp"

class WingedMesh;
class WingedFace;
class WingedEdge;

namespace SubdivUtil {
  /** `insertVertex (m,e,v)` inserts a new vertex `v` at edge `e`.
   * The new edge (from the first vertex of `e` to `v`) is returned.
   */
  WingedEdge& insertVertex (WingedMesh&, WingedEdge&, const glm::vec3&);

  /** `f' = triangulate6Gon(m,f)` triangulates the 6-gon `f`.
   * Note thate `f->edge ()->firstVertex (f)` must be an even vertex, i.e.
   * it must not be generated during the current subdivision step.
   * The new faces are adjacent to `f`.
   * `f` becomes invalid: use `f'` instead.
   */
  WingedFace& triangulate6Gon (WingedMesh&, WingedFace&);

  /** `triangulateQuadAtHeighestLevelVertex (m,f)` triangulates the quad `f` by 
   * inserting an edge from the vertex of `f` with the heighest level. */
  void triangulateQuadAtHeighestLevelVertex (WingedMesh&, WingedFace&);
}

#endif

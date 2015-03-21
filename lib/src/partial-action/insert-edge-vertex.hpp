#ifndef DILAY_PARTIAL_ACTION_INSERT_EDGE_VERTEX
#define DILAY_PARTIAL_ACTION_INSERT_EDGE_VERTEX

class WingedEdge;
class WingedMesh;

namespace PartialAction {

  /** `insertEdgeVertex (m,e,v)` inserts a new vertex `v` at edge `e`.
   * The new edge `n` (from the first vertex of `e` to `v`) is returned.
   */
  WingedEdge& insertEdgeVertex (WingedMesh&, WingedEdge&, const glm::vec3&);
};
#endif

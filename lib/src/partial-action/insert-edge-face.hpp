#ifndef DILAY_PARTIAL_ACTION_INSERT_EDGE_FACE
#define DILAY_PARTIAL_ACTION_INSERT_EDGE_FACE

class WingedMesh;
class WingedEdge;
class WingedFace;

namespace PartialAction {

  /** `run (m,f)` inserts a new edge `e` from `f.edge ()->secondVertex (f)`
   * and a new triangle on the left-hand side of `e`.
   */
  WingedEdge& insertEdgeFace (WingedMesh&, WingedFace&);
}
#endif

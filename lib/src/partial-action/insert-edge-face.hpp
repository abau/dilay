#ifndef DILAY_PARTIAL_ACTION_INSERT_EDGE_FACE
#define DILAY_PARTIAL_ACTION_INSERT_EDGE_FACE

class WingedEdge;
class WingedFace;
class WingedMesh;
class WingedVertex;

namespace PartialAction {

  /** `insertEdgeFace (m,f,e,v)` inserts a new edge `n` from `e.secondVertex (f)`
   * to `v` and a new triangle on the left-hand side of `n`.
   * Behavior is undefined if `e` is no edge of `f` or
   * the left-hand side of `n` forms no triangle.
   */
  WingedEdge& insertEdgeFace (WingedMesh&, WingedFace&, WingedEdge&, WingedVertex&);

  /** `insertEdgeFace (m,f)` equals 
   * `insertEdgeFace (m, f, f.edgeRef (), f.edgeRef ().predecessorRef (f).firstVertexRef (f))`.
   */
  WingedEdge& insertEdgeFace (WingedMesh&, WingedFace&);
}
#endif

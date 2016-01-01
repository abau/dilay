/* This file is part of Dilay
 * Copyright © 2015,2016 Alexander Bau
 * Use and redistribute under the terms of the GNU General Public License
 */
#ifndef DILAY_PARTIAL_ACTION_INSERT_EDGE_FACE
#define DILAY_PARTIAL_ACTION_INSERT_EDGE_FACE

class WingedEdge;
class WingedFace;
class WingedMesh;
class WingedVertex;

namespace PartialAction {

  /** `insertEdgeFace (m,f,e)` inserts a new edge `n` from `e.secondVertex (f)`
   * to `e.predecessorRef (f).firstVertexRef (f)` and a new triangle on the left-hand side of `n`.
   * Behavior is undefined if `e` is no edge of `f` or
   * the left-hand side of `n` forms no triangle.
   */
  WingedEdge& insertEdgeFace (WingedMesh&, WingedFace&, WingedEdge&);

  /** `insertEdgeFace (m,f)` equals `insertEdgeFace (m, f, f.edgeRef ())`.
   */
  WingedEdge& insertEdgeFace (WingedMesh&, WingedFace&);
}
#endif

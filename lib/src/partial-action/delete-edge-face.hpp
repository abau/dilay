#ifndef DILAY_PARTIAL_ACTION_DELETE_EDGE_FACE
#define DILAY_PARTIAL_ACTION_DELETE_EDGE_FACE

class AffectedFaces;
class WingedEdge;
class WingedMesh;

namespace PartialAction {

  /* `deleteEdgeFace (m,e,a)` deletes edge `e` and its _right_ face of mesh `m`. 
   * `e` and its right face are also deleted from `a`.
   */
  void deleteEdgeFace (WingedMesh&, WingedEdge&, AffectedFaces&);
}

#endif

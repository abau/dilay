#ifndef DILAY_PARTIAL_ACTION_DELETE_EDGE_FACE
#define DILAY_PARTIAL_ACTION_DELETE_EDGE_FACE

#include "action/on.hpp"
#include "macro.hpp"

class WingedEdge;
class WingedMesh;

class PADeleteEdgeFace : public ActionOn <WingedMesh> {
  public: 
    DECLARE_BIG3 (PADeleteEdgeFace)

    // `deleteEdgeFace (m,e)` deletes edge `e` and its _right_ face of mesh `m`. 
    void deleteEdgeFace (WingedMesh&, WingedEdge&);

    // `dissolveEdgeFace (e)` dissolves edge `e` and its _right_ face without deleting them.
    void dissolveEdgeFace (WingedEdge&);

  private:
    void runUndo (WingedMesh&);
    void runRedo (WingedMesh&);

    IMPLEMENTATION
};
#endif

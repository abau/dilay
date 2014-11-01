#ifndef DILAY_PARTIAL_ACTION_DELETE_EDGE_FACE
#define DILAY_PARTIAL_ACTION_DELETE_EDGE_FACE

#include "action/on.hpp"
#include "fwd-winged.hpp"
#include "macro.hpp"

class AffectedFaces;

class PADeleteEdgeFace : public ActionOn <WingedMesh> {
  public: 
    DECLARE_BIG3 (PADeleteEdgeFace)

    /* `run (m,e,a)` deletes edge `e` and its _right_ face of mesh `m`. 
     * `e` and its right face are also deleted from `a` if `a != nullptr`.
     */
    void run (WingedMesh&, WingedEdge&, AffectedFaces*);
    void run (WingedMesh&, EdgePtrVec&, AffectedFaces*);

  private:
    void runUndo (WingedMesh&);
    void runRedo (WingedMesh&);

    IMPLEMENTATION
};
#endif

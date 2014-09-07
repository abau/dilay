#ifndef DILAY_PARTIAL_ACTION_DELETE_EDGE_FACE
#define DILAY_PARTIAL_ACTION_DELETE_EDGE_FACE

#include "action/on.hpp"
#include "macro.hpp"

class WingedEdge;
class WingedMesh;

class PADeleteEdgeFace : public ActionOn <WingedMesh> {
  public: 
    DECLARE_BIG3 (PADeleteEdgeFace)

    /** `run (m,e)` deletes edge `e` and its _right_ face of mesh `m`. 
     * Note that other parts of the program depend on this behaviour. */
    void run (WingedMesh&, WingedEdge&);

  private:
    void runUndo (WingedMesh&);
    void runRedo (WingedMesh&);

    IMPLEMENTATION
};
#endif

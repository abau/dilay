#ifndef DILAY_PARTIAL_ACTION_DELETE_EDGE_FACE
#define DILAY_PARTIAL_ACTION_DELETE_EDGE_FACE

#include "action/on-winged-mesh.hpp"
#include "fwd-glm.hpp"
#include "macro.hpp"

class WingedEdge;
class WingedMesh;

class PADeleteEdgeFace : public ActionOnWMesh {
  public: 
    DECLARE_ACTION_BIG6 (PADeleteEdgeFace)

    /** `run (m,e)` deletes edge `e` and its _right_ face of mesh `m`. 
     * Note that other parts of the program depend on this behaviour. */
    void run (WingedMesh&, WingedEdge&);

    void undo (WingedMesh&);
    void redo (WingedMesh&);
  private:
    class Impl;
    Impl* impl;
};
#endif

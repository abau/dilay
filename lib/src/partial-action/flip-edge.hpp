#ifndef DILAY_PARTIAL_ACTION_FLIP_EDGE
#define DILAY_PARTIAL_ACTION_FLIP_EDGE

#include "action/on-winged-mesh.hpp"
#include "macro.hpp"

class WingedMesh;
class WingedEdge;

class PAFlipEdge : public ActionOnWMesh {
  public: 
    DECLARE_BIG3 (PAFlipEdge)

    void run (WingedMesh&, WingedEdge&);

  private:
    void runUndo (WingedMesh&) const;
    void runRedo (WingedMesh&) const;

    IMPLEMENTATION
};
#endif

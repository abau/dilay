#ifndef DILAY_ACTION_RELAX_EDGE
#define DILAY_ACTION_RELAX_EDGE

#include "action/on.hpp"
#include "macro.hpp"

class WingedMesh;
class WingedEdge;
class AffectedFaces;

class ActionRelaxEdge : public ActionOn <WingedMesh> {
  public: 
    DECLARE_BIG3 (ActionRelaxEdge)

    void run (WingedMesh&, WingedEdge&, AffectedFaces&);

  private:
    void runUndo (WingedMesh&);
    void runRedo (WingedMesh&);

    IMPLEMENTATION
};

#endif

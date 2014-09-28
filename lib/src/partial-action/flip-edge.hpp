#ifndef DILAY_PARTIAL_ACTION_FLIP_EDGE
#define DILAY_PARTIAL_ACTION_FLIP_EDGE

#include "action/on.hpp"
#include "macro.hpp"

class WingedMesh;
class WingedEdge;

class PAFlipEdge : public ActionOn <WingedMesh> {
  public: 
    DECLARE_BIG3 (PAFlipEdge)

    void run (WingedEdge&);

  private:
    void runUndo (WingedMesh&);
    void runRedo (WingedMesh&);

    IMPLEMENTATION
};
#endif

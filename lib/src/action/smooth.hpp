#ifndef DILAY_ACTION_SMOOTH
#define DILAY_ACTION_SMOOTH

#include <unordered_set>
#include "action/on.hpp"
#include "fwd-winged.hpp"
#include "macro.hpp"

class AffectedFaces;

class ActionSmooth : public ActionOn <WingedMesh> {
  public: 
    DECLARE_BIG3 (ActionSmooth)

    void run (WingedMesh&, const VertexSet&, AffectedFaces&);
  private:
    void runUndo (WingedMesh&);
    void runRedo (WingedMesh&);

    IMPLEMENTATION
};

#endif

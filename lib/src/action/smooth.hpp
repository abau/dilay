#ifndef DILAY_ACTION_SMOOTH
#define DILAY_ACTION_SMOOTH

#include "action/on.hpp"
#include "fwd-winged.hpp"
#include "macro.hpp"

class AffectedFaces;

class ActionSmooth : public ActionOn <WingedMesh> {
  public: 
    DECLARE_BIG3 (ActionSmooth)

    void run (WingedMesh&, const VertexPtrSet&, unsigned int, AffectedFaces&);
  private:
    void runUndo (WingedMesh&) const;
    void runRedo (WingedMesh&) const;

    IMPLEMENTATION
};

#endif

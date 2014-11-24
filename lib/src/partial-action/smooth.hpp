#ifndef DILAY_PARTIAL_ACTION_SMOOTH
#define DILAY_PARTIAL_ACTION_SMOOTH

#include "action/on.hpp"
#include "fwd-winged.hpp"
#include "macro.hpp"

class AffectedFaces;

class PASmooth : public ActionOn <WingedMesh> {
  public: 
    DECLARE_BIG3 (PASmooth)

    void run (WingedMesh&, const VertexPtrSet&, unsigned int, AffectedFaces&);
  private:
    void runUndo (WingedMesh&) const;
    void runRedo (WingedMesh&) const;

    IMPLEMENTATION
};

#endif

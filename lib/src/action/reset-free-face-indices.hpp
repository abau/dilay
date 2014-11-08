#ifndef DILAY_ACTION_RESET_FREE_FACE_INDICES
#define DILAY_ACTION_RESET_FREE_FACE_INDICES

#include "action/on.hpp"
#include "macro.hpp"

class WingedMesh;

class ActionResetFreeFaceIndices : public ActionOn <WingedMesh> {
  public:
    DECLARE_BIG3 (ActionResetFreeFaceIndices)

    void run (WingedMesh&);

  private:
    void runUndo (WingedMesh&);
    void runRedo (WingedMesh&);

    IMPLEMENTATION
};

#endif

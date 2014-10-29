#ifndef DILAY_ACTION_COLLAPSE_FACE
#define DILAY_ACTION_COLLAPSE_FACE

#include "action/on.hpp"
#include "macro.hpp"

class WingedMesh;
class WingedFace;
class AffectedFaces;

class ActionCollapseFace : public ActionOn <WingedMesh> {
  public:
    DECLARE_BIG3 (ActionCollapseFace)

    void run (WingedMesh&, WingedFace&, AffectedFaces&);

  private:
    void runUndo (WingedMesh&);
    void runRedo (WingedMesh&);

    IMPLEMENTATION
};

#endif

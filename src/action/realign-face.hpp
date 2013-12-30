#ifndef DILAY_ACTION_REALIGN_FACE
#define DILAY_ACTION_REALIGN_FACE

#include "action/on-winged-mesh.hpp"
#include "macro.hpp"

class WingedMesh;
class WingedFace;

class ActionRealignFace : public ActionOnWMesh {
  public: 
    DECLARE_ACTION_BIG6 (ActionRealignFace)

    WingedFace& run  (WingedMesh&, const WingedFace&);
    void        undo (WingedMesh&);
    void        redo (WingedMesh&);
  private:
    class Impl;
    Impl* impl;
};
#endif

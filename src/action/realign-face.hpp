#ifndef DILAY_ACTION_REALIGN_FACE
#define DILAY_ACTION_REALIGN_FACE

#include "action.hpp"
#include "macro.hpp"

class WingedMesh;
class WingedFace;

class ActionRealignFace : public Action {
  public: 
    DECLARE_ACTION_BIG5 (ActionRealignFace)

    WingedFace& run  (WingedMesh&, const WingedFace&);
    void        undo ();
    void        redo ();
  private:
    class Impl;
    Impl* impl;
};
#endif

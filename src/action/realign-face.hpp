#ifndef DILAY_ACTION_REALIGN_FACE
#define DILAY_ACTION_REALIGN_FACE

#include "action.hpp"

class WingedMesh;
class WingedFace;

class ActionRealignFace : public Action {
  public: ActionRealignFace            ();
          ActionRealignFace            (const ActionRealignFace&) = delete;
    const ActionRealignFace& operator= (const ActionRealignFace&) = delete;
         ~ActionRealignFace            ();

    WingedFace& run  (WingedMesh&, const WingedFace&);
    void        undo ();
    void        redo ();
  private:
    class Impl;
    Impl* impl;
};
#endif

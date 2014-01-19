#ifndef DILAY_ACTION_ON_POST_PROCESSED_WINGED_MESH
#define DILAY_ACTION_ON_POST_PROCESSED_WINGED_MESH

#include "action/on-winged-mesh.hpp"
#include "macro.hpp"

class WingedMesh;
class WingedFace;

class ActionOnPostProcessedWMesh : public ActionOnWMesh {
  public:
    DECLARE_ACTION_BIG6_VIRTUAL (ActionOnPostProcessedWMesh)

  protected:
    WingedFace& realignFace (WingedMesh&, const WingedFace&);

  private:
    void runUndo (WingedMesh&);
    void runRedo (WingedMesh&);

    virtual void runUndoBeforePostProcessing (WingedMesh&) = 0;
    virtual void runRedoBeforePostProcessing (WingedMesh&) = 0;

    class Impl;
    Impl* impl;
};

#endif

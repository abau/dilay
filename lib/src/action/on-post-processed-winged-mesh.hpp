#ifndef DILAY_ACTION_ON_POST_PROCESSED_WINGED_MESH
#define DILAY_ACTION_ON_POST_PROCESSED_WINGED_MESH

#include "action/on.hpp"
#include "macro.hpp"

class WingedMesh;
class WingedFace;

class ActionOnPostProcessedWMesh : public ActionOn <WingedMesh> {
  public:
    DECLARE_BIG3_VIRTUAL (ActionOnPostProcessedWMesh)

    void writeMesh  (bool);
    void bufferMesh (bool);
    bool writeMesh  () const;
    bool bufferMesh () const;

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

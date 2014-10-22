#ifndef DILAY_ACTION_ON_POST_PROCESSED_WINGED_MESH
#define DILAY_ACTION_ON_POST_PROCESSED_WINGED_MESH

#include "action/on.hpp"
#include "macro.hpp"

class WingedMesh;
class WingedFace;

class ActionOnPostProcessedWMesh : public ActionOn <WingedMesh> {
  public:
    DECLARE_BIG3_VIRTUAL (ActionOnPostProcessedWMesh)

  protected:
    WingedFace& realignFace     (WingedMesh&, WingedFace&&);
    void        realignAllFaces (WingedMesh&);
    void        writeAllNormals (WingedMesh&);
    void        writeAllIndices (WingedMesh&);
    void        bufferData      (WingedMesh&);

  private:
    void runUndo (WingedMesh&);
    void runRedo (WingedMesh&);

    virtual void runUndoBeforePostProcessing (WingedMesh&) = 0;
    virtual void runRedoBeforePostProcessing (WingedMesh&) = 0;

    IMPLEMENTATION
};

#endif

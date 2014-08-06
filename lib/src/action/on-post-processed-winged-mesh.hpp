#ifndef DILAY_ACTION_ON_POST_PROCESSED_WINGED_MESH
#define DILAY_ACTION_ON_POST_PROCESSED_WINGED_MESH

#include "action/on.hpp"
#include "macro.hpp"

class WingedMesh;
class WingedFace;
class WingedVertex;

class ActionOnPostProcessedWMesh : public ActionOn <WingedMesh> {
  public:
    DECLARE_BIG3_VIRTUAL (ActionOnPostProcessedWMesh)

  protected:
    void        bufferData      (WingedMesh&);
    WingedFace& realignFace     (WingedMesh&, const WingedFace&);
    void        writeAllIndices (WingedMesh&);
    void        writeIndices    (WingedMesh&, WingedFace&);
    void        writeAllNormals (WingedMesh&);
    void        writeNormals    (WingedMesh&, WingedFace&);
    void        writeNormal     (WingedMesh&, WingedVertex&);

  private:
    void runUndo (WingedMesh&);
    void runRedo (WingedMesh&);

    virtual void runUndoBeforePostProcessing (WingedMesh&) = 0;
    virtual void runRedoBeforePostProcessing (WingedMesh&) = 0;

    class Impl;
    Impl* impl;
};

#endif

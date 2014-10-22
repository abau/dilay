#ifndef DILAY_ACTION_SUBDIVIDE_MESH
#define DILAY_ACTION_SUBDIVIDE_MESH

#include "action/on-post-processed-winged-mesh.hpp"
#include "macro.hpp"

class WingedMesh;

class ActionSubdivideMesh : public ActionOnPostProcessedWMesh {
  public: 
    DECLARE_BIG3 (ActionSubdivideMesh)

    void run (WingedMesh&);

  private:
    void runUndoBeforePostProcessing (WingedMesh&);
    void runRedoBeforePostProcessing (WingedMesh&);

    IMPLEMENTATION
};

#endif

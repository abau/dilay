#ifndef DILAY_ACTION_FROM_MESH
#define DILAY_ACTION_FROM_MESH

#include "action/on-post-processed-winged-mesh.hpp"
#include "macro.hpp"

class WingedMesh;
class Mesh;

class ActionFromMesh : public ActionOnPostProcessedWMesh {
  public: 
    DECLARE_BIG3 (ActionFromMesh)

    void run (WingedMesh&, const Mesh&);

  private:
    void runUndoBeforePostProcessing (WingedMesh&);
    void runRedoBeforePostProcessing (WingedMesh&);

    class Impl;
    Impl* impl;
};

#endif

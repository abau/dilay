#ifndef DILAY_ACTION_FROM_MESH
#define DILAY_ACTION_FROM_MESH

#include "action/on-post-processed-winged-mesh.hpp"
#include "macro.hpp"

class WingedMesh;

class ActionFromMesh : public ActionOnPostProcessedWMesh {
  public: 
    DECLARE_ACTION_BIG6 (ActionFromMesh)

    void cube      (WingedMesh&);
    void sphere    (WingedMesh&, unsigned int, unsigned int);
    void icosphere (WingedMesh&, unsigned int);

  private:
    void runUndoBeforePostProcessing (WingedMesh&);
    void runRedoBeforePostProcessing (WingedMesh&);

    class Impl;
    Impl* impl;
};

#endif

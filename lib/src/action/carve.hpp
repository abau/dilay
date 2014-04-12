#ifndef DILAY_ACTION_CARVE
#define DILAY_ACTION_CARVE

#include <list>
#include "action/on-post-processed-winged-mesh.hpp"
#include "macro.hpp"

class Id;
class WingedMesh;

class ActionCarve : public ActionOnPostProcessedWMesh {
  public: 
    DECLARE_BIG6 (ActionCarve)

    void run (WingedMesh&, const glm::vec3&, float);

  private:
    void runUndoBeforePostProcessing (WingedMesh&);
    void runRedoBeforePostProcessing (WingedMesh&);

    class Impl;
    Impl* impl;
};

#endif

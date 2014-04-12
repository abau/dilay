#ifndef DILAY_ACTION_SUBDIVIDE
#define DILAY_ACTION_SUBDIVIDE

#include <list>
#include "action/on-post-processed-winged-mesh.hpp"
#include "macro.hpp"

class WingedFace;
class WingedMesh;
class Id;

class ActionSubdivide : public ActionOnPostProcessedWMesh {
  public: 
    DECLARE_BIG3 (ActionSubdivide)

    void run (WingedMesh&, WingedFace&, std::list <Id>* = nullptr);

  private:
    void runUndoBeforePostProcessing (WingedMesh&);
    void runRedoBeforePostProcessing (WingedMesh&);

    class Impl;
    Impl* impl;
};

#endif

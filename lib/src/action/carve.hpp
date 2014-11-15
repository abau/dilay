#ifndef DILAY_ACTION_CARVE
#define DILAY_ACTION_CARVE

#include "action/on-post-processed-winged-mesh.hpp"
#include "macro.hpp"

class WingedMesh;
class CarveBrush;

class ActionCarve : public ActionOnPostProcessedWMesh {
  public: 
    DECLARE_BIG3 (ActionCarve)

    void run (const CarveBrush&);

  private:
    void runUndoBeforePostProcessing (WingedMesh&) const;
    void runRedoBeforePostProcessing (WingedMesh&) const;

    IMPLEMENTATION
};

#endif

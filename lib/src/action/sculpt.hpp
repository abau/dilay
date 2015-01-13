#ifndef DILAY_ACTION_SCULPT
#define DILAY_ACTION_SCULPT

#include "action/on-post-processed-winged-mesh.hpp"
#include "macro.hpp"

class SculptBrush;
class WingedMesh;

class ActionSculpt : public ActionOnPostProcessedWMesh {
  public: 
    DECLARE_BIG3 (ActionSculpt)

    void run (const SculptBrush&);

  private:
    void runUndoBeforePostProcessing (WingedMesh&) const;
    void runRedoBeforePostProcessing (WingedMesh&) const;

    IMPLEMENTATION
};

#endif

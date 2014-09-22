#ifndef DILAY_ACTION_SUBDIVIDE
#define DILAY_ACTION_SUBDIVIDE

#include <unordered_set>
#include <vector>
#include "action/on-post-processed-winged-mesh.hpp"
#include "macro.hpp"

class WingedEdge;
class WingedMesh;
class Id;

class ActionSubdivide : public ActionOnPostProcessedWMesh {
  public: 
    DECLARE_BIG3 (ActionSubdivide)

    void run (WingedMesh&, const std::unordered_set <Id>&, std::vector <Id>*);

  private:
    void runUndoBeforePostProcessing (WingedMesh&);
    void runRedoBeforePostProcessing (WingedMesh&);

    IMPLEMENTATION
};

#endif

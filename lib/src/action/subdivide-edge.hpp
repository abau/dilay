#ifndef DILAY_ACTION_SUBDIVIDE_EDGE
#define DILAY_ACTION_SUBDIVIDE_EDGE

#include "action/on.hpp"
#include "macro.hpp"

class WingedMesh;
class WingedEdge;
class AffectedFaces;

class ActionSubdivideEdge : public ActionOn <WingedMesh> {
  public: 
    DECLARE_BIG3 (ActionSubdivideEdge)

    static void      extendDomain  (AffectedFaces&);
    static void      addOneRing    (AffectedFaces&);
           void      subdivideEdge (WingedMesh&, WingedEdge&, AffectedFaces&);
           void      relaxEdge     (WingedMesh&, WingedEdge&, AffectedFaces&);
  private:
    void runUndo (WingedMesh&);
    void runRedo (WingedMesh&);

    IMPLEMENTATION
};

#endif

#ifndef DILAY_ACTION_SUBDIVIDE
#define DILAY_ACTION_SUBDIVIDE

#include "action/on.hpp"
#include "macro.hpp"

class WingedMesh;
class WingedEdge;
class AffectedFaces;

class ActionSubdivide : public ActionOn <WingedMesh> {
  public: 
    DECLARE_BIG3 (ActionSubdivide)

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

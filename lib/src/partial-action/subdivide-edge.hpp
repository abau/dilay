#ifndef DILAY_PARTIAL_ACTION_SUBDIVIDE_EDGE
#define DILAY_PARTIAL_ACTION_SUBDIVIDE_EDGE

#include "action/on.hpp"
#include "macro.hpp"

class WingedMesh;
class WingedEdge;
class AffectedFaces;

class PASubdivideEdge : public ActionOn <WingedMesh> {
  public: 
    DECLARE_BIG3 (PASubdivideEdge)

    static void      extendDomain (AffectedFaces&);
           void      run          (WingedMesh&, WingedEdge&, AffectedFaces&);

  private:
    void runUndo (WingedMesh&) const;
    void runRedo (WingedMesh&) const;

    IMPLEMENTATION
};

#endif

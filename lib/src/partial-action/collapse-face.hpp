#ifndef DILAY_PARTIAL_ACTION_COLLAPSE_FACE
#define DILAY_PARTIAL_ACTION_COLLAPSE_FACE

#include "action/on-winged-mesh.hpp"
#include "macro.hpp"

class WingedMesh;
class WingedFace;
class AffectedFaces;

class PACollapseFace : public ActionOnWMesh {
  public:
    DECLARE_BIG3 (PACollapseFace)

    void run (WingedMesh&, WingedFace&, AffectedFaces&);

  private:
    void runUndo (WingedMesh&) const;
    void runRedo (WingedMesh&) const;

    IMPLEMENTATION
};

#endif

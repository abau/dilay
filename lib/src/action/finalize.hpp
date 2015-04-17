#ifndef DILAY_ACTION_FINALIZE
#define DILAY_ACTION_FINALIZE

class AffectedFaces;
class WingedMesh;

namespace Action {

  void collapseDegeneratedFaces (WingedMesh&);
  void collapseDegeneratedFaces (WingedMesh&, AffectedFaces&);
  void finalize                 (WingedMesh&, AffectedFaces&);
}

#endif

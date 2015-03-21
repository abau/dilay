#ifndef DILAY_PARTIAL_ACTION_SUBDIVIDE_EDGE
#define DILAY_PARTIAL_ACTION_SUBDIVIDE_EDGE

class WingedMesh;
class WingedEdge;
class AffectedFaces;

namespace PartialAction {

  void extendDomain  (AffectedFaces&);
  void subdivideEdge (WingedMesh&, WingedEdge&, AffectedFaces&);
}

#endif

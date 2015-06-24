#ifndef DILAY_PARTIAL_ACTION_COLLAPSE_EDGE
#define DILAY_PARTIAL_ACTION_COLLAPSE_EDGE

class AffectedFaces;
class WingedEdge;
class WingedMesh;

namespace PartialAction {

  bool collapseEdge (WingedMesh&, WingedEdge&, AffectedFaces&);
};

#endif

#ifndef DILAY_PARTIAL_ACTION_RELAX_EDGE
#define DILAY_PARTIAL_ACTION_RELAX_EDGE

class WingedMesh;
class WingedEdge;
class AffectedFaces;

namespace PartialAction {

  void relaxEdge (WingedMesh&, WingedEdge&, AffectedFaces&);
}

#endif

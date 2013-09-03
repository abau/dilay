#ifndef DILAY_SUBDIVISION_BUTTERFLY
#define DILAY_SUBDIVISION_BUTTERFLY

#include <list>

class Id;
class WingedMesh;
class WingedFace;
class WingedEdge;

namespace SubdivButterfly {
  void subdivide     (WingedMesh&, WingedEdge&, float = -1.0f);
  void subdivide     (WingedMesh&, WingedFace&);
  void subdivide     (WingedMesh&, std::list<Id>&);
}

#endif

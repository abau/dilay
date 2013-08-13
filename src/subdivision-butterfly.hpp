#ifndef DILAY_SUBDIVISION_BUTTERFLY
#define DILAY_SUBDIVISION_BUTTERFLY

#include <list>

class Id;
class WingedMesh;
class WingedFace;

namespace SubdivButterfly {
  void subdivide     (WingedMesh&, WingedFace&);
  void subdivide     (WingedMesh&, std::list<Id>&);
  void subdivideCopy (WingedMesh&, const std::list <Id>&);
}

#endif

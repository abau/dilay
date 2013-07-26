#ifndef DILAY_SUBDIVISION_BUTTERFLY
#define DILAY_SUBDIVISION_BUTTERFLY

#include <list>

class WingedMesh;
class WingedFace;

namespace SubdivButterfly {
  void subdivide (WingedMesh&, WingedFace&);
//  void subdivide (WingedMesh&, std::list<WingedFace*>&);
}

#endif

#ifndef DILAY_SUBDIVISION_BUTTERFLY
#define DILAY_SUBDIVISION_BUTTERFLY

#include <fwd-glm.hpp>

class WingedMesh;
class WingedEdge;

namespace SubdivisionButterfly {
  glm::vec3 subdivideEdge (const WingedMesh&, WingedEdge&);
}

#endif

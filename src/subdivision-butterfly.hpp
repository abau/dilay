#ifndef DILAY_SUBDIVISION_BUTTERFLY
#define DILAY_SUBDIVISION_BUTTERFLY

#include "fwd-glm.hpp"
#include "fwd-winged.hpp"

namespace SubdivButterfly {
  void      subdiv (WingedMesh&);
  glm::vec3 subdiv (const WingedMesh&, LinkedEdge);
}

#endif

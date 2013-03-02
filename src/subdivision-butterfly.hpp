#ifndef SUBDIVISION_BUTTERFLY
#define SUBDIVISION_BUTTERFLY

#include "subdivision-util.hpp"
#include "winged-mesh.hpp"
#include "winged-edge.hpp"

namespace SubdivButterfly {
  void      subdiv (WingedMesh&);
  glm::vec3 subdiv (WingedMesh&, LinkedEdge&);
}

#endif

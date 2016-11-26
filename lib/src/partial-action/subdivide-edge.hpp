/* This file is part of Dilay
 * Copyright © 2015,2016 Alexander Bau
 * Use and redistribute under the terms of the GNU General Public License
 */
#ifndef DILAY_PARTIAL_ACTION_SUBDIVIDE_EDGE
#define DILAY_PARTIAL_ACTION_SUBDIVIDE_EDGE

#include <glm/fwd.hpp>

class WingedMesh;
class WingedEdge;
class AffectedFaces;

namespace PartialAction {

  void extendDomain  (AffectedFaces&);
  void subdivideEdge (WingedMesh&, WingedEdge&, AffectedFaces&);
  void subdivideEdge (WingedMesh&, WingedEdge&, AffectedFaces&, const glm::vec3&);
}

#endif

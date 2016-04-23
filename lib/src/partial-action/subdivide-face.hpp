/* This file is part of Dilay
 * Copyright © 2015,2016 Alexander Bau
 * Use and redistribute under the terms of the GNU General Public License
 */
#ifndef DILAY_PARTIAL_ACTION_SUBDIVIDE_FACE
#define DILAY_PARTIAL_ACTION_SUBDIVIDE_FACE

#include <glm/fwd.hpp>

class AffectedFaces;
class WingedFace;
class WingedMesh;

namespace PartialAction {

  void subdivideFace (WingedMesh&, WingedFace&, AffectedFaces&, const glm::vec3&);
}

#endif

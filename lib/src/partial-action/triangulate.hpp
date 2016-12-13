/* This file is part of Dilay
 * Copyright © 2015,2016 Alexander Bau
 * Use and redistribute under the terms of the GNU General Public License
 */
#ifndef DILAY_PARTIAL_ACTION_TRIANGULATE
#define DILAY_PARTIAL_ACTION_TRIANGULATE

#include <glm/fwd.hpp>

class WingedMesh;
class WingedFace;
class AffectedFaces;

namespace PartialAction {
  void triangulateQuad     (WingedMesh&, WingedFace&, AffectedFaces&);
  void triangulate5Gon     (WingedMesh&, WingedFace&, AffectedFaces&);
  void triangulate6Gon     (WingedMesh&, WingedFace&, AffectedFaces&);
  void triangulate         (WingedMesh&, WingedFace&, AffectedFaces&);
  void splitAndTriangulate (WingedMesh&, WingedEdge&, const glm::vec3&, AffectedFaces&);
  void splitAndTriangulate (WingedMesh&, WingedFace&, const glm::vec3&, AffectedFaces&);
}
#endif

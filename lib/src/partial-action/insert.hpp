/* This file is part of Dilay
 * Copyright © 2015,2016 Alexander Bau
 * Use and redistribute under the terms of the GNU General Public License
 */
#ifndef DILAY_PARTIAL_ACTION_INSERT
#define DILAY_PARTIAL_ACTION_INSERT

#include <glm/fwd.hpp>

class AffectedFaces;
class WingedEdge;
class WingedMesh;

namespace PartialAction {
  WingedEdge& insertEdgeVertex (WingedMesh&, WingedEdge&, const glm::vec3&, AffectedFaces&);
  WingedEdge& insertEdgeFace (WingedMesh&, WingedFace&, WingedEdge&, WingedVertex&, AffectedFaces&);
  WingedEdge& insertEdgeFaceAtNewVertex (WingedMesh&, WingedFace&, WingedEdge&, AffectedFaces&);
};
#endif

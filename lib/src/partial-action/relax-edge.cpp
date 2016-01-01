/* This file is part of Dilay
 * Copyright © 2015,2016 Alexander Bau
 * Use and redistribute under the terms of the GNU General Public License
 */
#include <glm/glm.hpp>
#include "affected-faces.hpp"
#include "partial-action/flip-edge.hpp"
#include "partial-action/relax-edge.hpp"
#include "winged/edge.hpp"
#include "winged/vertex.hpp"

namespace {
  bool relaxableEdge (const WingedEdge& edge) {
    const int v1  = int (edge.vertex1Ref ().valence ());
    const int v2  = int (edge.vertex2Ref ().valence ());
    const int v3  = int (edge.vertexRef (edge.leftFaceRef  (), 2).valence ());
    const int v4  = int (edge.vertexRef (edge.rightFaceRef (), 2).valence ());

    const int pre  = glm::abs (v1-6)   + glm::abs (v2-6)   + glm::abs (v3-6)   + glm::abs (v4-6);
    const int post = glm::abs (v1-6-1) + glm::abs (v2-6-1) + glm::abs (v3-6+1) + glm::abs (v4-6+1);

    return (v1 > 3) && (v2 > 3) && (post < pre);
  }
}

void PartialAction :: relaxEdge (WingedMesh& mesh, WingedEdge& edge, AffectedFaces& affectedFaces) {
  if (relaxableEdge (edge)) {
    affectedFaces.insert (edge.leftFaceRef  ());
    affectedFaces.insert (edge.rightFaceRef ());
    PartialAction::flipEdge (mesh, edge);
  }
}

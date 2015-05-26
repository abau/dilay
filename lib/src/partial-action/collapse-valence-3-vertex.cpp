#include "adjacent-iterator.hpp"
#include "partial-action/collapse-valence-2-vertex.hpp"
#include "partial-action/collapse-valence-3-vertex.hpp"
#include "partial-action/delete-edge-face.hpp"
#include "winged/edge.hpp"
#include "winged/face.hpp"
#include "winged/vertex.hpp"

void PartialAction :: collapseValence3Vertex ( WingedMesh& mesh, WingedVertex& vertex
                                             , bool recursively
                                             , AffectedFaces& affectedFaces)
{
  assert (vertex.valence () == 3);

  PartialAction::deleteEdgeFace (mesh, vertex.edgeRef (), affectedFaces);

  WingedEdge& edgeToDelete  = PartialAction::collapseValence2Vertex ( mesh, vertex 
                                                                    , false, affectedFaces);
  WingedFace& remainingFace = edgeToDelete.leftFaceRef ();

  PartialAction::deleteEdgeFace (mesh, edgeToDelete, affectedFaces);

  assert (remainingFace.numEdges () == 3);
  remainingFace.writeIndices (mesh);

  if (recursively) {
    for (WingedVertex& v : remainingFace.adjacentVertices ()) {
      if (v.valence () == 3) {
        PartialAction::collapseValence3Vertex (mesh, v, recursively, affectedFaces);
        return; // !
      }
    }
  }
}

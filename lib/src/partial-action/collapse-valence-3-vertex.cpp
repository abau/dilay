#include "adjacent-iterator.hpp"
#include "affected-faces.hpp"
#include "partial-action/collapse-valence-3-vertex.hpp"
#include "partial-action/collapse-vertex.hpp"
#include "winged/face.hpp"
#include "winged/mesh.hpp"
#include "winged/vertex.hpp"

void PartialAction :: collapseValence3Vertex ( WingedMesh& mesh, WingedVertex& vertex
                                             , AffectedFaces& affectedFaces )
{
  assert (vertex.valence () == 3);

  for (const WingedVertex& v : vertex.adjacentVertices ()) {
    if (v.valence () == 3) {
      mesh         .reset ();
      affectedFaces.reset ();
      return;
    }
  }
  WingedFace& remainingFace = PartialAction::collapseVertex (mesh, vertex, affectedFaces);

  remainingFace.writeIndices (mesh);
}

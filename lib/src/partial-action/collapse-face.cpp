#include "affected-faces.hpp"
#include "partial-action/collapse-edge.hpp"
#include "partial-action/collapse-face.hpp"
#include "partial-action/delete-valence-3-vertex.hpp"
#include "winged/edge.hpp"
#include "winged/face.hpp"
#include "winged/mesh.hpp"
#include "winged/vertex.hpp"

void PartialAction :: collapseFace ( WingedMesh& mesh, WingedFace& face
                                   , AffectedFaces& affectedFaces ) 
{
  assert (face.numEdges () == 3);

  WingedVertex& vertex1 = face.vertexRef (0);
  WingedVertex& vertex2 = face.vertexRef (1);
  WingedVertex& vertex3 = face.vertexRef (2);

  const unsigned int valence1 = vertex1.valence ();
  const unsigned int valence2 = vertex2.valence ();
  const unsigned int valence3 = vertex3.valence ();

  assert (valence1 >= 3);
  assert (valence2 >= 3);
  assert (valence3 >= 3);

  if (valence1 == 3) {
    PartialAction::deleteValence3Vertex (mesh, vertex1, affectedFaces);
  }
  else if (valence2 == 3) {
    PartialAction::deleteValence3Vertex (mesh, vertex2, affectedFaces);
  }
  else if (valence3 == 3) {
    PartialAction::deleteValence3Vertex (mesh, vertex3, affectedFaces);
  }
  else {
    WingedEdge* edges[] = { face.edge ()
                          , face.edgeRef ().successor (face)
                          , face.edgeRef ().successor (face, 1) };

    float distances[] = { edges[0]->lengthSqr (mesh)
                        , edges[1]->lengthSqr (mesh)
                        , edges[2]->lengthSqr (mesh) };

    for (unsigned int i = 0; i < 2; i++) {
      if (distances[1] < distances[0]) {
        std::swap (edges[0], edges[1]);
        std::swap (distances[0], distances[1]);
      }
      if (distances[2] < distances[1]) {
        std::swap (edges[1], edges[2]);
        std::swap (distances[1], distances[2]);
      }
    }

    for (unsigned int i = 0; i < 3; i++) {
      if (PartialAction::collapseEdge (mesh, *edges[i], affectedFaces)) {
        return;
      }
    }
    mesh         .reset ();
    affectedFaces.reset ();
  }
}

#include "partial-action/collapse-edge.hpp"
#include "partial-action/collapse-face.hpp"
#include "partial-action/collapse-valence-3-vertex.hpp"
#include "winged/edge.hpp"
#include "winged/face.hpp"
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
    PartialAction::collapseValence3Vertex (mesh, vertex1, affectedFaces);
  }
  else if (valence2 == 3) {
    PartialAction::collapseValence3Vertex (mesh, vertex2, affectedFaces);
  }
  else if (valence3 == 3) {
    PartialAction::collapseValence3Vertex (mesh, vertex3, affectedFaces);
  }
  else {
    WingedEdge& e1 = face.edgeRef ();
    WingedEdge& e2 = e1.successorRef (face);
    WingedEdge& e3 = e2.successorRef (face);

    const float d1 = e1.lengthSqr (mesh);
    const float d2 = e2.lengthSqr (mesh);
    const float d3 = e3.lengthSqr (mesh);

    WingedEdge& shortest = d1 < d2 && d1 < d3
                         ? e1
                         : (d2 < d3 ? e2 : e3);

    PartialAction::collapseEdge (mesh, shortest, affectedFaces);
  }
}

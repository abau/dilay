#include "partial-action/collapse-edge.hpp"
#include "partial-action/collapse-face.hpp"
#include "winged/edge.hpp"
#include "winged/face.hpp"
#include "winged/vertex.hpp"

void PartialAction :: collapseFace ( WingedMesh& mesh, WingedFace& face
                                   , AffectedFaces& affectedFaces ) 
{
  assert (face.numEdges () == 3);

  assert (face.vertexRef (0).valence () > 3);
  assert (face.vertexRef (1).valence () > 3);
  assert (face.vertexRef (2).valence () > 3);

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

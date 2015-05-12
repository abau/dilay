#include "affected-faces.hpp"
#include "partial-action/insert-edge-face.hpp"
#include "partial-action/triangulate-quad.hpp"
#include "winged/edge.hpp"
#include "winged/face.hpp"

WingedEdge& PartialAction :: triangulateQuad ( WingedMesh& mesh, WingedFace& face
                                             , AffectedFaces& affectedFaces ) 
{
  assert (face.numEdges () == 4);

  WingedEdge& newEdge = PartialAction::insertEdgeFace (mesh, face);

  newEdge.leftFaceRef  ().writeIndices (mesh);
  newEdge.rightFaceRef ().writeIndices (mesh);

  affectedFaces.insert (newEdge.leftFaceRef  ());
  affectedFaces.insert (newEdge.rightFaceRef ());

  return newEdge;
}

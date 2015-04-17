#include "affected-faces.hpp"
#include "partial-action/collapse-valence-2-vertex.hpp"
#include "winged/edge.hpp"
#include "winged/face.hpp"
#include "winged/mesh.hpp"
#include "winged/vertex.hpp"

WingedEdge& PartialAction :: collapseValence2Vertex ( WingedMesh& mesh
                                                    , WingedVertex& vertex
                                                    , bool recursively
                                                    , AffectedFaces& affectedFaces )
{
  assert (vertex.valence () == 2);

  WingedEdge& remainingEdge = vertex.edgeRef ();
  WingedFace& left          = remainingEdge.leftFaceRef ();
  WingedFace& right         = remainingEdge.rightFaceRef ();
  WingedEdge& edgeToDelete  = remainingEdge.isVertex1 (vertex)
                            ? remainingEdge.leftPredecessorRef ()
                            : remainingEdge.leftSuccessorRef ();

  assert ( (left .index () == edgeToDelete.leftFaceRef  ().index ())
        || (left .index () == edgeToDelete.rightFaceRef ().index ()) );
  assert ( (right.index () == edgeToDelete.leftFaceRef  ().index ())
        || (right.index () == edgeToDelete.rightFaceRef ().index ()) );

  if (remainingEdge.isVertex1 (vertex)) {
    remainingEdge.leftPredecessor (edgeToDelete.predecessor (left));
    remainingEdge.rightSuccessor  (edgeToDelete.successor (right));
    remainingEdge.vertex1         (edgeToDelete.otherVertex (vertex));
  }
  else {
    remainingEdge.leftSuccessor    (edgeToDelete.successor (left));
    remainingEdge.rightPredecessor (edgeToDelete.predecessor (right));
    remainingEdge.vertex2          (edgeToDelete.otherVertex (vertex));
  }

  if (edgeToDelete.isVertex1 (vertex)) {
    edgeToDelete.leftSuccessorRef    ().predecessor (edgeToDelete.leftFaceRef  (), &remainingEdge);
    edgeToDelete.rightPredecessorRef ().successor   (edgeToDelete.rightFaceRef (), &remainingEdge);
  }
  else {
    edgeToDelete.leftPredecessorRef ().successor   (edgeToDelete.leftFaceRef  (), &remainingEdge);
    edgeToDelete.rightSuccessorRef  ().predecessor (edgeToDelete.rightFaceRef (), &remainingEdge);
  }

  remainingEdge.vertex1Ref ().edge (&remainingEdge);
  remainingEdge.vertex2Ref ().edge (&remainingEdge);

  left .edge (&remainingEdge);
  right.edge (&remainingEdge);

  affectedFaces.insert (left);
  affectedFaces.insert (right);

  mesh.deleteEdge   (edgeToDelete);
  mesh.deleteVertex (vertex);

  if (recursively) {
    if (remainingEdge.vertex1Ref ().valence () == 2) {
      return PartialAction::collapseValence2Vertex ( mesh, remainingEdge.vertex1Ref ()
                                                   , recursively, affectedFaces );
    }
    else if (remainingEdge.vertex2Ref ().valence () == 2) {
      return PartialAction::collapseValence2Vertex ( mesh, remainingEdge.vertex2Ref ()
                                                   , recursively, affectedFaces );
    }
  }
  return remainingEdge;
}

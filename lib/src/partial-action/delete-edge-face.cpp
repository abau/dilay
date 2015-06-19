#include "adjacent-iterator.hpp"
#include "affected-faces.hpp"
#include "partial-action/delete-edge-face.hpp"
#include "winged/edge.hpp"
#include "winged/face.hpp"
#include "winged/mesh.hpp"
#include "winged/vertex.hpp"

namespace {
  void dissolveEdgeFace (WingedEdge& edge) {
    WingedFace& faceToDelete  = *edge.rightFace ();
    WingedFace& remainingFace = *edge.leftFace ();

    for (WingedEdge* adjacent : faceToDelete.adjacentEdges ().collect ()) {
      adjacent->face (faceToDelete, &remainingFace);
    }

    edge.leftPredecessorRef  ().successor   (remainingFace, edge.rightSuccessor   ());
    edge.leftSuccessorRef    ().predecessor (remainingFace, edge.rightPredecessor ());

    edge.rightPredecessorRef ().successor   (remainingFace, edge.leftSuccessor    ());
    edge.rightSuccessorRef   ().predecessor (remainingFace, edge.leftPredecessor  ());

    edge.vertex1Ref ().edge (edge.leftPredecessor ());
    edge.vertex2Ref ().edge (edge.leftSuccessor   ());

    remainingFace.edge (edge.leftSuccessor ());
  }
}

void PartialAction :: deleteEdgeFace ( WingedMesh& mesh, WingedEdge& edge
                                     , AffectedFaces& affectedFaces) 
{
#ifndef NDEBUG
  const unsigned int numEdgesLeft  = edge.leftFaceRef  ().numEdges ();
  const unsigned int numEdgesRight = edge.rightFaceRef ().numEdges ();
#endif
  assert (numEdgesLeft  >= 2);
  assert (numEdgesRight >= 2);

  WingedFace& faceToDelete  = edge.rightFaceRef ();
  WingedFace& remainingFace = edge.leftFaceRef ();

  assert (faceToDelete != remainingFace);

  affectedFaces.remove (faceToDelete);
  affectedFaces.insert (remainingFace);

  dissolveEdgeFace (edge);

  mesh.deleteEdge (edge);
  mesh.deleteFace (faceToDelete); 

  assert (remainingFace.numEdges () >= 3);
  assert (remainingFace.numEdges () == numEdgesLeft + numEdgesRight - 2);
}

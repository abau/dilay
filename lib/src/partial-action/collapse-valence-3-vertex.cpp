#include "adjacent-iterator.hpp"
#include "affected-faces.hpp"
#include "partial-action/collapse-valence-3-vertex.hpp"
#include "partial-action/delete-edge-face.hpp"
#include "winged/edge.hpp"
#include "winged/face.hpp"
#include "winged/mesh.hpp"
#include "winged/vertex.hpp"

namespace {
  WingedEdge& collapseValence2Vertex ( WingedMesh& mesh, WingedVertex& vertex
                                     , AffectedFaces& affectedFaces )
  {
    assert (vertex.valence () == 2);

    WingedEdge& remainingEdge = vertex.edgeRef ();
    WingedFace& left          = remainingEdge.leftFaceRef ();
    WingedFace& right         = remainingEdge.rightFaceRef ();
    WingedEdge& edgeToDelete  = remainingEdge.isVertex1 (vertex)
                              ? remainingEdge.leftPredecessorRef ()
                              : remainingEdge.leftSuccessorRef ();

    assert (left  == edgeToDelete.leftFaceRef () || left  == edgeToDelete.rightFaceRef ());
    assert (right == edgeToDelete.leftFaceRef () || right == edgeToDelete.rightFaceRef ());
    assert ( (left.numEdges () == 3 && right.numEdges () == 4)
          || (left.numEdges () == 4 && right.numEdges () == 3) );

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

    assert ( (left.numEdges () == 2 && right.numEdges () == 3)
          || (left.numEdges () == 3 && right.numEdges () == 2) );

    return remainingEdge;
  }
}

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
  PartialAction::deleteEdgeFace (mesh, vertex.edgeRef (), affectedFaces);

  WingedEdge& edgeToDelete  = collapseValence2Vertex (mesh, vertex, affectedFaces);
  WingedFace& remainingFace = edgeToDelete.leftFaceRef ();

  PartialAction::deleteEdgeFace (mesh, edgeToDelete, affectedFaces);

  assert (remainingFace.numEdges () == 3);
  remainingFace.writeIndices (mesh);
}

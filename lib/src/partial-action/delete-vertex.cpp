/* This file is part of Dilay
 * Copyright © 2015 Alexander Bau
 * Use and redistribute under the terms of the GNU General Public License
 */
#include "affected-faces.hpp"
#include "partial-action/delete-edge-face.hpp"
#include "partial-action/delete-vertex.hpp"
#include "winged/edge.hpp"
#include "winged/face.hpp"
#include "winged/mesh.hpp"
#include "winged/vertex.hpp"

namespace {
  WingedEdge& deleteValence2Vertex ( WingedMesh& mesh, WingedVertex& vertex
                                   , AffectedFaces& affectedFaces )
  {
    assert (vertex.valence () == 2);

    WingedEdge& remainingEdge = vertex.edgeRef ();
    WingedFace& left          = remainingEdge.leftFaceRef ();
    WingedFace& right         = remainingEdge.rightFaceRef ();
    WingedEdge& edgeToDelete  = remainingEdge.isVertex1 (vertex)
                              ? remainingEdge.leftPredecessorRef ()
                              : remainingEdge.leftSuccessorRef ();
#ifndef NDEBUG
    const unsigned int numEdgesLeft  = left.numEdges ();
    const unsigned int numEdgesRight = right.numEdges ();
#endif
    assert (numEdgesLeft  >= 3);
    assert (numEdgesRight >= 3);

    assert (left  == edgeToDelete.leftFaceRef () || left  == edgeToDelete.rightFaceRef ());
    assert (right == edgeToDelete.leftFaceRef () || right == edgeToDelete.rightFaceRef ());

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

    assert (remainingEdge.leftFaceRef ().numEdges () == numEdgesLeft - 1);
    assert (remainingEdge.rightFaceRef ().numEdges () == numEdgesRight - 1);

    return remainingEdge;
  }
}

WingedFace& PartialAction :: deleteVertex ( WingedMesh& mesh, WingedVertex& vertex
                                          , AffectedFaces& affectedFaces )
{
  const unsigned int valence = vertex.valence ();

  assert (valence >= 2);

  for (unsigned int v = valence; v > 2; v--) {
    PartialAction::deleteEdgeFace (mesh, vertex.edgeRef (), affectedFaces);
  }
  assert (vertex.valence () == 2);

  WingedEdge& edgeToDelete  = deleteValence2Vertex (mesh, vertex, affectedFaces);
  WingedFace& remainingFace = edgeToDelete.leftFaceRef ();

  PartialAction::deleteEdgeFace (mesh, edgeToDelete, affectedFaces);
  return remainingFace;
}

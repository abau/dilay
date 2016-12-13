/* This file is part of Dilay
 * Copyright © 2015,2016 Alexander Bau
 * Use and redistribute under the terms of the GNU General Public License
 */
#include "adjacent-iterator.hpp"
#include "affected-faces.hpp"
#include "partial-action/delete.hpp"
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

namespace PartialAction {
  void deleteEdgeAndRightFace (WingedMesh& mesh, WingedEdge& edge, AffectedFaces& affectedFaces) {
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

  WingedFace& deleteVertex (WingedMesh& mesh, WingedVertex& vertex, AffectedFaces& affectedFaces) {
    const unsigned int valence = vertex.valence ();

    assert (valence >= 2);

    for (unsigned int v = valence; v > 2; v--) {
      PartialAction::deleteEdgeAndRightFace (mesh, vertex.edgeRef (), affectedFaces);
    }
    assert (vertex.valence () == 2);

    WingedEdge& edgeToDelete  = deleteValence2Vertex (mesh, vertex, affectedFaces);
    WingedFace& remainingFace = edgeToDelete.leftFaceRef ();

    PartialAction::deleteEdgeAndRightFace (mesh, edgeToDelete, affectedFaces);
    return remainingFace;
  }

  void deleteValence3Vertex (WingedMesh& mesh, WingedVertex& vertex, AffectedFaces& affectedFaces) {
    assert (vertex.valence () == 3);

    for (const WingedVertex& v : vertex.adjacentVertices ()) {
      if (v.valence () == 3) {
        mesh         .reset ();
        affectedFaces.reset ();
        return;
      }
    }
    WingedFace& remainingFace = PartialAction::deleteVertex (mesh, vertex, affectedFaces);

    remainingFace.writeIndices (mesh);
  }
}

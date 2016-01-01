/* This file is part of Dilay
 * Copyright © 2015,2016 Alexander Bau
 * Use and redistribute under the terms of the GNU General Public License
 */
#include <glm/glm.hpp>
#include "adjacent-iterator.hpp"
#include "affected-faces.hpp"
#include "partial-action/collapse-edge.hpp"
#include "partial-action/delete-edge-face.hpp"
#include "partial-action/delete-valence-3-vertex.hpp"
#include "winged/edge.hpp"
#include "winged/face.hpp"
#include "winged/mesh.hpp"
#include "winged/vertex.hpp"

namespace {

  bool splitsMesh (WingedVertex& v1, WingedVertex& v2, WingedVertex& v3, WingedVertex& v4) {
    VertexPtrVec v1Adjacent = v1.adjacentVertices ().collect ();
    VertexPtrVec v2Adjacent = v2.adjacentVertices ().collect ();

    for (WingedVertex* v1Adj : v1Adjacent) {
      if (*v1Adj != v3 && *v1Adj != v4) {
        for (WingedVertex* v2Adj : v2Adjacent) {
          if (*v1Adj == *v2Adj) {
            return true;
          }
        }
      }
    }
    return false;
  }
}

bool PartialAction::collapseEdge ( WingedMesh& mesh, WingedEdge& edge
                                 , AffectedFaces& affectedFaces ) 
{
  assert (edge.leftFaceRef  ().numEdges () == 3);
  assert (edge.rightFaceRef ().numEdges () == 3);

  WingedVertex& vertex1       = edge.vertex1Ref ();
  WingedVertex& vertex2       = edge.vertex2Ref ();
  WingedEdge&   edgeToDelete1 = edge.leftSuccessorRef ();
  WingedEdge&   edgeToDelete2 = edge.rightSuccessorRef ();
  WingedVertex& vertex3       = edgeToDelete1.otherVertexRef (vertex2);
  WingedVertex& vertex4       = edgeToDelete2.otherVertexRef (vertex1);

  const unsigned int edgeIndex = edge.index ();
  const unsigned int valence1  = vertex1.valence ();
  const unsigned int valence2  = vertex2.valence ();
  const unsigned int valence3  = vertex3.valence ();
  const unsigned int valence4  = vertex4.valence ();

  if (valence1 == 3) {
    PartialAction::deleteValence3Vertex (mesh, vertex1, affectedFaces);
    return true;
  }
  else if (valence2 == 3) {
    PartialAction::deleteValence3Vertex (mesh, vertex2, affectedFaces);
    return true;
  }
  else if (valence3 == 3) {
    PartialAction::deleteValence3Vertex (mesh, vertex3, affectedFaces);

    if (mesh.edge (edgeIndex)) {
      return PartialAction::collapseEdge (mesh, edge, affectedFaces);
    }
    else {
      return true;
    }
  }
  else if (valence4 == 3) {
    PartialAction::deleteValence3Vertex (mesh, vertex4, affectedFaces);

    if (mesh.edge (edgeIndex)) {
      return PartialAction::collapseEdge (mesh, edge, affectedFaces);
    }
    else {
      return true;
    }
  }
  else if (splitsMesh (vertex1, vertex2, vertex3, vertex4)) {
    return false;
  }
  else {
    WingedVertex& newVertex = mesh.addVertex (edge.middle (mesh));

    PartialAction::deleteEdgeFace (mesh, edgeToDelete1, affectedFaces);
    PartialAction::deleteEdgeFace (mesh, edgeToDelete2, affectedFaces);

    assert (edge.leftFaceRef ().numEdges () == 4);
    assert (edge.rightFaceRef ().numEdges () == 4);

    EdgePtrVec remainingEdges1 = vertex1.adjacentEdges ().collect ();
    EdgePtrVec remainingEdges2 = vertex2.adjacentEdges ().collect ();

    WingedFace& left  = edge.leftFaceRef ();
    WingedFace& right = edge.rightFaceRef ();
    WingedEdge& lp    = edge.leftPredecessorRef ();
    WingedEdge& ls    = edge.leftSuccessorRef ();
    WingedEdge& rp    = edge.rightPredecessorRef ();
    WingedEdge& rs    = edge.rightSuccessorRef ();

    lp.successor   (left , &ls);
    ls.predecessor (left , &lp);
    rp.successor   (right, &rs);
    rs.predecessor (right, &rp);

    newVertex.edge (&ls);
    left     .edge (&ls);
    right    .edge (&rs);

    for (WingedEdge* e : remainingEdges1) {
      e->vertex (vertex1, &newVertex);
    }
    for (WingedEdge* e : remainingEdges2) {
      e->vertex (vertex2, &newVertex);
    }

    mesh.deleteEdge   (edge);
    mesh.deleteVertex (vertex1);
    mesh.deleteVertex (vertex2);

    for (WingedFace& f : newVertex.adjacentFaces ()) {
      assert (f.numEdges () == 3);
      affectedFaces.insert (f);
      f.writeIndices (mesh);
    }
    assert (newVertex.valence () == (valence1 - 3) + (valence2 - 3) + 2);

#ifndef NDEBUG
    for (WingedVertex& v : newVertex.adjacentVertices ()) {
      assert (v.valence () > 2);
    }
#endif
    return true;
  }
}

#include "affected-faces.hpp"
#include "adjacent-iterator.hpp"
#include "partial-action/collapse-face.hpp"
#include "partial-action/delete-edge-face.hpp"
#include "primitive/triangle.hpp"
#include "winged/edge.hpp"
#include "winged/face.hpp"
#include "winged/mesh.hpp"
#include "winged/vertex.hpp"

namespace {

  bool isCollapsable (const WingedFace& face) {
    unsigned int num3Valences = 0;
    for (WingedVertex& v : face.adjacentVertices ()) {
      num3Valences += v.valence () == 3 ? 1 : 0;
    }
    return num3Valences <= 1;
  }

  void deleteSuccessors (WingedMesh& mesh, WingedFace& face, AffectedFaces& affectedFaces) {
    for (WingedEdge& e : face.adjacentEdges ()) {
      PartialAction::deleteEdgeFace (mesh, e.successorRef (e.otherFaceRef (face)), &affectedFaces);
    }
  }

  WingedVertex& addNewVertex (WingedMesh& mesh, WingedFace& face) {
    WingedEdge& edge         = face.edgeRef        ();
    WingedFace& otherFace    = edge.otherFaceRef   (face);
    WingedEdge& fPredecessor = edge.predecessorRef (face);
    WingedEdge& fSuccessor   = edge.successorRef   (face);
    WingedEdge& oPredecessor = edge.predecessorRef (otherFace);
    WingedEdge& oSuccessor   = edge.successorRef   (otherFace);

    WingedVertex& newVertex = mesh.addVertex (face.triangle (mesh).center ());

    if (fSuccessor.index () != oPredecessor.index ()) {
      newVertex.edge (&oPredecessor);
    }
    else {
      assert (fPredecessor.index () != oSuccessor.index ());
      newVertex.edge (&oSuccessor);
    }
    return newVertex;
  }

  void reassign (WingedMesh& mesh, WingedFace& face, WingedVertex& newVertex) {
    VertexPtrVec verticesToDelete = face.adjacentVertices ().collect ();
    EdgePtrVec   edgesToDelete    = face.adjacentEdges    ().collect ();

    // handle vertices
    for (WingedVertex* v : verticesToDelete) {
      for (WingedEdge* e : v->adjacentEdges ().collect ()) {
        e->vertex (*v, &newVertex);
      }
      mesh.deleteVertex (*v);
    }
    // handle edges
    for (WingedEdge* e : edgesToDelete) {
      WingedFace& otherFace   = e->otherFaceRef   (face);
      WingedEdge& predecessor = e->predecessorRef (otherFace);
      WingedEdge& successor   = e->successorRef   (otherFace);

      predecessor.successor (otherFace, &successor);
      successor.predecessor (otherFace, &predecessor);
      otherFace.edge        (&successor);
    }
    // delete old edges & face
    for (WingedEdge* e : edgesToDelete) {
      mesh.deleteEdge (*e);
    }
    mesh.deleteFace (face);

    // write indices
    for (WingedFace& f : newVertex.adjacentFaces ()) {
      f.writeIndices (mesh);
    }
  }

  void addAffectedFaces (WingedVertex& newVertex, AffectedFaces& affectedFaces) {
    for (WingedFace& f : newVertex.adjacentFaces ()) {
      affectedFaces.insert (f);
    }
  }
}

void PartialAction :: collapseFace ( WingedMesh& mesh, WingedFace& face
                                   , AffectedFaces& affectedFaces ) 
{
  if (isCollapsable (face)) {
    affectedFaces.remove (face);
    deleteSuccessors (mesh, face, affectedFaces);

    WingedVertex& newVertex = addNewVertex (mesh, face);

    reassign         (mesh, face, newVertex);
    addAffectedFaces (newVertex, affectedFaces);
  }
  else {
    std::abort ();
  }
}

#include <glm/glm.hpp>
#include "action/collapse-face.hpp"
#include "action/unit/on.hpp"
#include "adjacent-iterator.hpp"
#include "affected-faces.hpp"
#include "partial-action/delete-edge-face.hpp"
#include "partial-action/modify-winged-edge.hpp"
#include "partial-action/modify-winged-face.hpp"
#include "partial-action/modify-winged-mesh.hpp"
#include "partial-action/modify-winged-vertex.hpp"
#include "primitive/triangle.hpp"
#include "winged/edge.hpp"
#include "winged/face.hpp"
#include "winged/mesh.hpp"
#include "winged/vertex.hpp"

struct ActionCollapseFace::Impl {
  ActionUnitOn <WingedMesh> actions;

  void runUndo (WingedMesh& mesh) { this->actions.undo (mesh); }
  void runRedo (WingedMesh& mesh) { this->actions.redo (mesh); }

  void run (WingedMesh& mesh, WingedFace& face, AffectedFaces& affectedFaces) {
    if (this->isCollapsable (face)) {
      this->deleteFromAffectedFaces (face, affectedFaces);
      this->deleteSuccessors        (mesh, face, affectedFaces);

      WingedVertex& newVertex = this->addNewVertex (mesh, face);

      this->reassign         (mesh, face, newVertex);
      this->addAffectedFaces (newVertex, affectedFaces);
    }
    else {
      assert (false);
    }
  }

  bool isCollapsable (const WingedFace& face) const {
    unsigned int num3Valences = 0;
    for (WingedVertex& v : face.adjacentVertices ()) {
      num3Valences += v.valence () == 3 ? 1 : 0;
    }
    return num3Valences <= 1;
  }
  
  void deleteFromAffectedFaces (WingedFace& face, AffectedFaces& affectedFaces) {
    affectedFaces.remove (face);
    for (WingedEdge& e : face.adjacentEdges ()) {
      affectedFaces.remove (e);
    }
  }

  void deleteSuccessors (WingedMesh& mesh, WingedFace& face, AffectedFaces& affectedFaces) {
    EdgePtrVec successorsToDelete;
    for (WingedEdge& e : face.adjacentEdges ()) {
      successorsToDelete.push_back (e.successor (e.otherFaceRef (face)));
    }
    this->actions.add <PADeleteEdgeFace> ().run (mesh, successorsToDelete, &affectedFaces);
  }

  WingedVertex& addNewVertex (WingedMesh& mesh, WingedFace& face) {
    WingedVertex& newVertex    = mesh.addVertex (face.triangle (mesh).center ());
    WingedEdge&   edge         = face.edgeRef        ();
    WingedFace&   otherFace    = edge.otherFaceRef   (face);
    WingedEdge&   fPredecessor = edge.predecessorRef (face);
    WingedEdge&   fSuccessor   = edge.successorRef   (face);
    WingedEdge&   oPredecessor = edge.predecessorRef (otherFace);
    WingedEdge&   oSuccessor   = edge.successorRef   (otherFace);

    if (fSuccessor.id () != oPredecessor.id ()) {
      this->actions.add <PAModifyWVertex> ().edge (newVertex, &oPredecessor);
    }
    else {
      assert (fPredecessor.id () != oSuccessor.id ());
      this->actions.add <PAModifyWVertex> ().edge (newVertex, &oSuccessor);
    }
    return newVertex;
  }

  void reassign (WingedMesh& mesh, WingedFace& face, WingedVertex& newVertex) {
    VertexPtrVec  verticesToDelete = face.adjacentVertices ().collect ();
    EdgePtrVec    edgesToDelete    = face.adjacentEdges    ().collect ();
    PrimTriangle  ....

    // handle vertices
    for (WingedVertex* v : verticesToDelete) {
      for (WingedEdge* e : v->adjacentEdges ().collect ()) {
        this->actions.add <PAModifyWEdge> ().vertex (*e, *v, &newVertex);
      }
      this->actions.add <PAModifyWMesh> ().deleteVertex (mesh, *v);
    }
    // handle edges
    for (WingedEdge* e : edgesToDelete) {
      WingedFace& otherFace   = e->otherFaceRef   (face);
      WingedEdge& predecessor = e->predecessorRef (otherFace);
      WingedEdge& successor   = e->successorRef   (otherFace);

      this->actions.add <PAModifyWEdge> ().successor   (predecessor, otherFace, &successor);
      this->actions.add <PAModifyWEdge> ().predecessor (successor, otherFace, &predecessor);
      this->actions.add <PAModifyWFace> ().edge        (otherFace, &successor);
      this->actions.add <PAModifyWMesh> ().deleteEdge  (mesh, *e);
    }
    this->actions.add <PAModifyWMesh> ().deleteFace (mesh, face);

    // write indices
    for (WingedFace& f : newVertex.adjacentFaces ()) {
      this->actions.add <PAModifyWFace> ().writeIndices (mesh, f);
    }
  }

  void addAffectedFaces (WingedVertex& newVertex, AffectedFaces& affectedFaces) {
    for (WingedFace& f : newVertex.adjacentFaces ()) {
      affectedFaces.insert (f);
    }
  }
};

DELEGATE_BIG3 (ActionCollapseFace)
DELEGATE3 (void, ActionCollapseFace, run, WingedMesh&, WingedFace&, AffectedFaces&)
DELEGATE1 (void, ActionCollapseFace, runUndo, WingedMesh&)
DELEGATE1 (void, ActionCollapseFace, runRedo, WingedMesh&)

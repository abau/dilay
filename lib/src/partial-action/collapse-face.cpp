#include <glm/glm.hpp>
#include "action/unit/on.hpp"
#include "adjacent-iterator.hpp"
#include "affected-faces.hpp"
#include "partial-action/collapse-face.hpp"
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

struct PACollapseFace::Impl {
  ActionUnitOn <WingedMesh> actions;

  void runUndo (WingedMesh& mesh) const { this->actions.undo (mesh); }
  void runRedo (WingedMesh& mesh) const { this->actions.redo (mesh); }

  void run (WingedMesh& mesh, WingedFace& face, AffectedFaces& affectedFaces) {
    if (this->isCollapsable (face)) {
      affectedFaces.remove   (face);
      this->deleteSuccessors (mesh, face, affectedFaces);

      WingedVertex& newVertex = this->addNewVertex (mesh, face);

      this->reassign         (mesh, face, newVertex);
      this->addAffectedFaces (newVertex, affectedFaces);
    }
    else {
      std::abort ();
    }
  }

  bool isCollapsable (const WingedFace& face) const {
    unsigned int num3Valences = 0;
    for (WingedVertex& v : face.adjacentVertices ()) {
      num3Valences += v.valence () == 3 ? 1 : 0;
    }
    return num3Valences <= 1;
  }
  
  void deleteSuccessors (WingedMesh& mesh, WingedFace& face, AffectedFaces& affectedFaces) {
    for (WingedEdge& e : face.adjacentEdges ()) {
      this->actions.add <PADeleteEdgeFace> ()
                   .run (mesh, e.successorRef (e.otherFaceRef (face)), &affectedFaces);
    }
  }

  WingedVertex& addNewVertex (WingedMesh& mesh, WingedFace& face) {
    WingedEdge&   edge         = face.edgeRef        ();
    WingedFace&   otherFace    = edge.otherFaceRef   (face);
    WingedEdge&   fPredecessor = edge.predecessorRef (face);
    WingedEdge&   fSuccessor   = edge.successorRef   (face);
    WingedEdge&   oPredecessor = edge.predecessorRef (otherFace);
    WingedEdge&   oSuccessor   = edge.successorRef   (otherFace);

    WingedVertex& newVertex = this->actions.add <PAModifyWMesh> ()
                                           .addVertex (mesh, face.triangle (mesh).center ());

    if (fSuccessor.index () != oPredecessor.index ()) {
      this->actions.add <PAModifyWVertex> ().edge (newVertex, &oPredecessor);
    }
    else {
      assert (fPredecessor.index () != oSuccessor.index ());
      this->actions.add <PAModifyWVertex> ().edge (newVertex, &oSuccessor);
    }
    return newVertex;
  }

  void reassign (WingedMesh& mesh, WingedFace& face, WingedVertex& newVertex) {
    VertexPtrVec  verticesToDelete = face.adjacentVertices ().collect ();
    EdgePtrVec    edgesToDelete    = face.adjacentEdges    ().collect ();

    // handle vertices
    for (WingedVertex* v : verticesToDelete) {
      for (WingedEdge* e : v->adjacentEdges ().collect ()) {
        this->actions.add <PAModifyWEdge> ().vertex (*e, *v, &newVertex);
      }
      this->actions.add <PAModifyWVertex> ().reset        (*v);
      this->actions.add <PAModifyWMesh>   ().deleteVertex (mesh, *v);
    }
    // handle edges
    for (WingedEdge* e : edgesToDelete) {
      WingedFace& otherFace   = e->otherFaceRef   (face);
      WingedEdge& predecessor = e->predecessorRef (otherFace);
      WingedEdge& successor   = e->successorRef   (otherFace);

      this->actions.add <PAModifyWEdge> ().successor   (predecessor, otherFace, &successor);
      this->actions.add <PAModifyWEdge> ().predecessor (successor, otherFace, &predecessor);
      this->actions.add <PAModifyWFace> ().edge        (otherFace, &successor);
      this->actions.add <PAModifyWEdge> ().reset       (*e);
    }
    this->actions.add <PAModifyWFace> ().reset (face);

    // delete old edges & face
    for (WingedEdge* e : edgesToDelete) {
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

DELEGATE_BIG3   (PACollapseFace)
DELEGATE3       (void, PACollapseFace, run, WingedMesh&, WingedFace&, AffectedFaces&)
DELEGATE1_CONST (void, PACollapseFace, runUndo, WingedMesh&)
DELEGATE1_CONST (void, PACollapseFace, runRedo, WingedMesh&)

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
    WingedVertex& newVertex = mesh.addVertex (face.triangle (mesh).center ());

    for (WingedVertex* v : face.adjacentVertices ().collect ()) {
      for (WingedEdge* e : v->adjacentEdges ().collect ()) {
        this->actions.add <PAModifyWEdge> ().vertex (*e, *v, &newVertex);
      }
      this->actions.add <PAModifyWMesh> ().deleteVertex (mesh, *v);
    }
    bool first = true;
    for (WingedEdge* e : face.adjacentEdges ().collect ()) {
      this->actions.add <PADeleteEdgeFace> ()
                   .run (mesh, e->successorRef (e->otherFaceRef (face)));

      WingedFace& otherFace   = e->otherFaceRef   (face);
      WingedEdge& predecessor = e->predecessorRef (otherFace);
      WingedEdge& successor   = e->successorRef   (otherFace);

      if (first) {
        this->actions.add <PAModifyWVertex> ().edge (newVertex, &successor);
        first = false;
      }
      this->actions.add <PAModifyWEdge> ().successor   (predecessor, otherFace, &successor);
      this->actions.add <PAModifyWEdge> ().predecessor (successor, otherFace, &predecessor);
      this->actions.add <PAModifyWFace> ().edge        (otherFace, &successor);
      this->actions.add <PAModifyWMesh> ().deleteEdge  (mesh, *e);
    }
    this->actions.add <PAModifyWMesh> ().deleteFace (mesh, face);

    for (WingedFace& f : newVertex.adjacentFaces ()) {
      affectedFaces.insert (f);
    }
  }
};

DELEGATE_BIG3 (ActionCollapseFace)
DELEGATE3 (void, ActionCollapseFace, run, WingedMesh&, WingedFace&, AffectedFaces&)
DELEGATE1 (void, ActionCollapseFace, runUndo, WingedMesh&)
DELEGATE1 (void, ActionCollapseFace, runRedo, WingedMesh&)

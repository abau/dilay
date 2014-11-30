#include "action/unit/on.hpp"
#include "adjacent-iterator.hpp"
#include "affected-faces.hpp"
#include "partial-action/delete-edge-face.hpp"
#include "partial-action/modify-winged-edge.hpp"
#include "partial-action/modify-winged-face.hpp"
#include "partial-action/modify-winged-mesh.hpp"
#include "partial-action/modify-winged-vertex.hpp"
#include "winged/edge.hpp"
#include "winged/face.hpp"
#include "winged/mesh.hpp"
#include "winged/vertex.hpp"

struct PADeleteEdgeFace :: Impl {
  ActionUnitOn <WingedMesh> actions;

  void run (WingedMesh& mesh, WingedEdge& edge, AffectedFaces* affectedFaces) {
    WingedFace& faceToDelete  = *edge.rightFace ();
    WingedFace& remainingFace = *edge.leftFace ();

    if (affectedFaces) {
      affectedFaces->remove (faceToDelete);
      affectedFaces->insert (remainingFace);
    }
    this->dissolveEdgeFace (edge);

    actions.add <PAModifyWEdge> ().reset      (edge);
    actions.add <PAModifyWFace> ().reset      (faceToDelete);
    actions.add <PAModifyWMesh> ().deleteEdge (mesh,edge);
    actions.add <PAModifyWMesh> ().deleteFace (mesh,faceToDelete); 
  }

  void dissolveEdgeFace (WingedEdge& edge) {
    WingedFace& faceToDelete  = *edge.rightFace ();
    WingedFace& remainingFace = *edge.leftFace ();

    assert (faceToDelete.octreeNode ());

    for (WingedEdge* adjacent : faceToDelete.adjacentEdges ().collect ()) {
      actions.add <PAModifyWEdge> ().face (*adjacent, faceToDelete, &remainingFace);
    }

    actions.add <PAModifyWEdge> ().successor 
      (edge.leftPredecessorRef (), remainingFace, edge.rightSuccessor ());
    actions.add <PAModifyWEdge> ().predecessor 
      (edge.leftSuccessorRef (), remainingFace, edge.rightPredecessor ());

    actions.add <PAModifyWEdge> ().successor
      (edge.rightPredecessorRef (), remainingFace, edge.leftSuccessor ());
    actions.add <PAModifyWEdge> ().predecessor 
      (edge.rightSuccessorRef (), remainingFace, edge.leftPredecessor ());

    actions.add <PAModifyWVertex> ().edge (edge.vertex1Ref (), edge.leftPredecessor ());
    actions.add <PAModifyWVertex> ().edge (edge.vertex2Ref (), edge.leftSuccessor   ());

    actions.add <PAModifyWFace> ().edge (remainingFace, edge.leftSuccessor ());
  }

  void runUndo (WingedMesh& mesh) const { this->actions.undo (mesh); }
  void runRedo (WingedMesh& mesh) const { this->actions.redo (mesh); }
};

DELEGATE_BIG3 (PADeleteEdgeFace)

DELEGATE3       (void, PADeleteEdgeFace, run    , WingedMesh&, WingedEdge&, AffectedFaces*)
DELEGATE1_CONST (void, PADeleteEdgeFace, runUndo, WingedMesh&)
DELEGATE1_CONST (void, PADeleteEdgeFace, runRedo, WingedMesh&)

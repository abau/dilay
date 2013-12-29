#include "partial-action/delete-edge-face.hpp"
#include "winged-vertex.hpp"
#include "winged-edge.hpp"
#include "winged-face.hpp"
#include "winged-mesh.hpp"
#include "action/unit.hpp"
#include "adjacent-iterator.hpp"
#include "partial-action/modify-winged-edge.hpp"
#include "partial-action/modify-winged-mesh.hpp"
#include "partial-action/modify-winged-face.hpp"
#include "partial-action/modify-winged-vertex.hpp"
#include "triangle.hpp"

struct PADeleteEdgeFace :: Impl {
  ActionUnit actions;

  void run (WingedMesh& mesh, WingedEdge& edge) {
    this->actions.reset ();

    WingedFace& faceToDelete  = *edge.rightFace ();
    WingedFace& remainingFace = *edge.leftFace ();
    Triangle    triangle      = faceToDelete.triangle (mesh);

    assert (faceToDelete.octreeNode ());

    for (auto it = faceToDelete.adjacentEdgeIterator (); it.isValid (); ) {
      WingedEdge& adjacent = it.element ();
      it.next ();
      actions.add <PAModifyWEdge> ()->face (mesh, adjacent, faceToDelete, &remainingFace);
    }

    actions.add <PAModifyWEdge> ()->successor 
      (mesh, edge.leftPredecessorRef (), remainingFace, edge.rightSuccessor ());
    actions.add <PAModifyWEdge> ()->predecessor 
      (mesh, edge.leftSuccessorRef (), remainingFace, edge.rightPredecessor ());

    actions.add <PAModifyWEdge> ()->successor
      (mesh, edge.rightPredecessorRef (), remainingFace, edge.leftSuccessor ());
    actions.add <PAModifyWEdge> ()->predecessor 
      (mesh, edge.rightSuccessorRef (), remainingFace, edge.leftPredecessor ());

    actions.add <PAModifyWVertex> ()->edge (mesh, edge.vertex1Ref (), edge.leftPredecessor ());
    actions.add <PAModifyWVertex> ()->edge (mesh, edge.vertex2Ref (), edge.leftSuccessor   ());

    if (edge.previousSibling ())
      actions.add <PAModifyWEdge> ()->nextSibling (mesh, edge.previousSiblingRef (), nullptr);
    if (edge.nextSibling ())
      actions.add <PAModifyWEdge> ()->previousSibling (mesh, edge.nextSiblingRef (), nullptr);

    actions.add <PAModifyWFace> ()->edge (mesh, remainingFace, edge.leftSuccessor ());

    actions.add <PAModifyWFace> ()->edge       (mesh, faceToDelete, nullptr);
    actions.add <PAModifyWEdge> ()->rightFace  (mesh, edge, nullptr);

    actions.add <PAModifyWMesh> ()->deleteEdge (mesh,edge);
    actions.add <PAModifyWMesh> ()->deleteFace (mesh,faceToDelete,triangle); 
  }

  void undo () { this->actions.undo (); }
  void redo () { this->actions.redo (); }
};

DELEGATE_ACTION_BIG5 (PADeleteEdgeFace)

DELEGATE2 (void,PADeleteEdgeFace,run,WingedMesh&,WingedEdge&)
DELEGATE  (void,PADeleteEdgeFace,undo)
DELEGATE  (void,PADeleteEdgeFace,redo)


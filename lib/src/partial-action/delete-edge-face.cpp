#include "action/unit/on.hpp"
#include "adjacent-iterator.hpp"
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

struct PADeleteEdgeFace :: Impl {
  ActionUnitOn <WingedMesh> actions;

  void deleteEdgeFace (WingedMesh& mesh, WingedEdge& edge) {
    WingedFace&  faceToDelete = *edge.rightFace ();
    PrimTriangle triangle     = faceToDelete.triangle (mesh);

    this->dissolveEdgeFace (edge);

    actions.add <PAModifyWMesh> ().deleteEdge (mesh,edge);
    actions.add <PAModifyWMesh> ().deleteFace (mesh,faceToDelete,triangle); 
  }

  void dissolveEdgeFace (WingedEdge& edge) {
    assert (this->actions.isEmpty ());

    WingedFace&  faceToDelete  = *edge.rightFace ();
    WingedFace&  remainingFace = *edge.leftFace ();

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

  void runUndo (WingedMesh& mesh) { this->actions.undo (mesh); }
  void runRedo (WingedMesh& mesh) { this->actions.redo (mesh); }
};

DELEGATE_BIG3 (PADeleteEdgeFace)

DELEGATE2 (void,PADeleteEdgeFace,deleteEdgeFace,WingedMesh&,WingedEdge&)
DELEGATE1 (void,PADeleteEdgeFace,dissolveEdgeFace,WingedEdge&)
DELEGATE1 (void,PADeleteEdgeFace,runUndo,WingedMesh&)
DELEGATE1 (void,PADeleteEdgeFace,runRedo,WingedMesh&)

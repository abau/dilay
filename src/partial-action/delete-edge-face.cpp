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

    // if (edge.previousSibling ())
    //   edge.previousSiblingRef ().nextSibling (edge.nextSibling ());
    // if (edge.nextSibling ())
    //   edge.nextSiblingRef ().previousSibling (edge.previousSibling ());

    actions.add <PAModifyWFace> ()->edge (mesh, remainingFace, edge.leftSuccessor ());

    actions.add <PAModifyWFace> ()->edge       (mesh, faceToDelete, nullptr);
    actions.add <PAModifyWEdge> ()->rightFace  (mesh, edge, nullptr);

    actions.add <PAModifyWMesh> ()->deleteEdge (mesh,edge);
    actions.add <PAModifyWMesh> ()->deleteFace (mesh,faceToDelete,triangle); 
  }


  /*
  void run (WingedMesh& mesh, WingedEdge& edge) {
    this->meshId.reset      (new Id (mesh.id ()));
    this->edgeId.reset      (new Id (edge.id ()));
    this->leftFaceId.reset  (new Id (edge.leftFaceRef         ().id ()));
    this->rightFaceId.reset (new Id (edge.rightFaceRef        ().id ()));
    this->leftPredId.reset  (new Id (edge.leftPredecessorRef  ().id ()));
    this->rightPredId.reset (new Id (edge.rightPredecessorRef ().id ()));
    this->leftSuccId.reset  (new Id (edge.leftSuccessorRef    ().id ()));
    this->rightSuccId.reset (new Id (edge.rightSuccessorRef   ().id ()));
    if (edge.previousSibling ())
      this->prevSibId.reset (new Id (edge.previousSiblingRef  ().id ()));
    else
      this->prevSibId.reset (nullptr);
    if (edge.nextSibling ())
      this->nextSibId.reset (new Id (edge.nextSiblingRef      ().id ()));
    else
      this->nextSibId.reset (nullptr);

    this->vertex1Index      = edge.vertex1Ref   ().index    ();
    this->vertex2Index      = edge.vertex2Ref   ().index    ();
    this->rightFaceGeometry = edge.rightFaceRef ().triangle (mesh);

    WingedFace* faceToDelete  = edge.rightFace ();
    WingedFace* remainingFace = edge.leftFace ();

    assert (faceToDelete->octreeNode ());

    for (auto it = faceToDelete->adjacentEdgeIterator (); it.isValid (); ) {
      WingedEdge& adjacent = it.element ();
      it.next ();
      adjacent.face (*faceToDelete, remainingFace);
    }

    edge.leftPredecessorRef  ().successor   (*remainingFace, edge.rightSuccessor   ());
    edge.leftSuccessorRef    ().predecessor (*remainingFace, edge.rightPredecessor ());

    edge.rightPredecessorRef ().successor   (*remainingFace, edge.leftSuccessor   ());
    edge.rightSuccessorRef   ().predecessor (*remainingFace, edge.leftPredecessor ());

    edge.vertex1Ref ().edge (edge.leftPredecessor ());
    edge.vertex2Ref ().edge (edge.leftSuccessor   ());

    if (edge.previousSibling ())
      edge.previousSiblingRef ().nextSibling (nullptr);
    if (edge.nextSibling ())
      edge.nextSiblingRef ().previousSibling (nullptr);

    // if (edge.previousSibling ())
    //   edge.previousSiblingRef ().nextSibling (edge.nextSibling ());
    // if (edge.nextSibling ())
    //   edge.nextSiblingRef ().previousSibling (edge.previousSibling ());

    remainingFace->edge (edge.leftSuccessor ());

    mesh.releaseFirstIndexNumber (*remainingFace);
    mesh.releaseFirstIndexNumber (*faceToDelete);
    mesh.deleteEdge              (edge);
    mesh.deleteFace              (*faceToDelete); 
  }
  */

  void undo () { this->actions.undo (); }
  void redo () { this->actions.redo (); }
};

DELEGATE_ACTION_BIG5 (PADeleteEdgeFace)

DELEGATE2 (void,PADeleteEdgeFace,run,WingedMesh&,WingedEdge&)
DELEGATE  (void,PADeleteEdgeFace,undo)
DELEGATE  (void,PADeleteEdgeFace,redo)


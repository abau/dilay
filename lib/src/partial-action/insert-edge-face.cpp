#include "action/unit/on.hpp"
#include "partial-action/modify-winged-edge.hpp"
#include "partial-action/modify-winged-face.hpp"
#include "partial-action/modify-winged-mesh.hpp"
#include "partial-action/insert-edge-face.hpp"
#include "primitive/triangle.hpp"
#include "winged/edge.hpp"
#include "winged/face.hpp"
#include "winged/mesh.hpp"

struct PAInsertEdgeFace :: Impl {
  ActionUnitOn <WingedMesh> actions;

  WingedEdge& run (WingedMesh& mesh, WingedFace& face) {
    WingedEdge&   edge        = face.edgeRef ();
    WingedEdge&   predecessor = edge.predecessorRef (face);

    PrimTriangle newLeftGeometry (mesh, predecessor.vertexRef (face, 0)
                                      , predecessor.vertexRef (face, 1)
                                      , predecessor.vertexRef (face, 2));

    return splitFaceWith ( mesh, newLeftGeometry, face
                         , edge, predecessor
                         , predecessor.predecessorRef (face), edge.successorRef (face) );
  }

  WingedEdge& splitFaceWith ( WingedMesh& mesh
                            , const PrimTriangle& newLeftGeometry, WingedFace& faceToSplit
                            , WingedEdge& leftPred,  WingedEdge& leftSucc
                            , WingedEdge& rightPred, WingedEdge& rightSucc) 
  {
    WingedFace& newLeft    = this->actions.add <PAModifyWMesh> ().addFace (mesh, newLeftGeometry);
    WingedEdge& splitAlong = this->actions.add <PAModifyWMesh> ().addEdge (mesh);

    this->actions.add <PAModifyWFace> ().edge (newLeft    , &splitAlong);
    this->actions.add <PAModifyWFace> ().edge (faceToSplit, &splitAlong);

    this->actions.add <PAModifyWEdge> ().
      vertex1 (splitAlong, leftPred.secondVertex (faceToSplit));
    this->actions.add <PAModifyWEdge> ().
      vertex2 (splitAlong, leftSucc.firstVertex (faceToSplit));

    this->actions.add <PAModifyWEdge> ().leftFace  (splitAlong, &newLeft);
    this->actions.add <PAModifyWEdge> ().rightFace (splitAlong, &faceToSplit);

    this->actions.add <PAModifyWEdge> ().leftPredecessor  (splitAlong, &leftPred);
    this->actions.add <PAModifyWEdge> ().leftSuccessor    (splitAlong, &leftSucc);
    this->actions.add <PAModifyWEdge> ().rightPredecessor (splitAlong, &rightPred);
    this->actions.add <PAModifyWEdge> ().rightSuccessor   (splitAlong, &rightSucc);

    this->actions.add <PAModifyWEdge> ().face        (leftPred, faceToSplit, &newLeft);
    this->actions.add <PAModifyWEdge> ().successor   (leftPred, newLeft, &splitAlong);

    this->actions.add <PAModifyWEdge> ().face        (leftSucc, faceToSplit, &newLeft);
    this->actions.add <PAModifyWEdge> ().predecessor (leftSucc, newLeft, &splitAlong);

    this->actions.add <PAModifyWEdge> ().successor   (rightPred, faceToSplit, &splitAlong);
    this->actions.add <PAModifyWEdge> ().predecessor (rightSucc, faceToSplit, &splitAlong);

    return splitAlong;
  }

  void runUndo (WingedMesh& mesh) const { this->actions.undo (mesh); }
  void runRedo (WingedMesh& mesh) const { this->actions.redo (mesh); }
};

DELEGATE_BIG3   (PAInsertEdgeFace)
DELEGATE2       (WingedEdge&, PAInsertEdgeFace, run, WingedMesh&, WingedFace&)
DELEGATE1_CONST (void, PAInsertEdgeFace, runUndo, WingedMesh&)
DELEGATE1_CONST (void, PAInsertEdgeFace, runRedo, WingedMesh&)

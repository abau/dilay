#include "action/unit/on.hpp"
#include "partial-action/modify-winged-edge.hpp"
#include "partial-action/modify-winged-face.hpp"
#include "partial-action/modify-winged-mesh.hpp"
#include "partial-action/triangulate-quad.hpp"
#include "primitive/triangle.hpp"
#include "winged/edge.hpp"
#include "winged/face.hpp"
#include "winged/mesh.hpp"

struct PATriangulateQuad :: Impl {
  ActionUnitOn <WingedMesh> actions;

  WingedEdge& run ( WingedMesh& mesh, WingedFace& face
                  , std::unordered_set <WingedFace*>* affectedFaces ) 
  {
    assert (face.numEdges () == 4);

    WingedEdge&   edge        = face.edgeRef ();
    WingedEdge&   counterpart = edge.successorRef (face,1);

    PrimTriangle newLeftGeometry (mesh, edge.vertexRef (face, 0)
                                      , edge.vertexRef (face, 1)
                                      , edge.vertexRef (face, 3));

    WingedFace& newFace = this->actions.add <PAModifyWMesh> ().addFace (mesh, newLeftGeometry);

    WingedEdge& newEdge = splitFaceWith ( mesh, newFace, face
                                        , edge, edge.predecessorRef (face)
                                        , counterpart, edge.successorRef (face) );

    this->actions.add <PAModifyWFace> ().writeIndices (mesh, newFace);
    this->actions.add <PAModifyWFace> ().writeIndices (mesh, face);

    if (affectedFaces) {
      affectedFaces->insert (&face);
      affectedFaces->insert (&newFace);
    }
    return newEdge;
  }

  WingedEdge& splitFaceWith ( WingedMesh& mesh , WingedFace& newLeft, WingedFace& faceToSplit
                            , WingedEdge& leftPred,  WingedEdge& leftSucc
                            , WingedEdge& rightPred, WingedEdge& rightSucc) 
  {
    WingedEdge& splitAlong = this->actions.add <PAModifyWMesh> ()
                                          .addEdge (mesh, WingedEdge ());

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
    this->actions.add <PAModifyWEdge> ().predecessor (leftPred, newLeft, &leftSucc);
    this->actions.add <PAModifyWEdge> ().successor   (leftPred, newLeft, &splitAlong);

    this->actions.add <PAModifyWEdge> ().face        (leftSucc, faceToSplit, &newLeft);
    this->actions.add <PAModifyWEdge> ().predecessor (leftSucc, newLeft, &splitAlong);
    this->actions.add <PAModifyWEdge> ().successor   (leftSucc, newLeft, &leftPred);

    this->actions.add <PAModifyWEdge> ().predecessor (rightPred, faceToSplit, &rightSucc);
    this->actions.add <PAModifyWEdge> ().successor   (rightPred, faceToSplit, &splitAlong);

    this->actions.add <PAModifyWEdge> ().predecessor (rightSucc, faceToSplit, &splitAlong);
    this->actions.add <PAModifyWEdge> ().successor   (rightSucc, faceToSplit, &rightPred);

    return splitAlong;
  }

  void runUndo (WingedMesh& mesh) { this->actions.undo (mesh); }
  void runRedo (WingedMesh& mesh) { this->actions.redo (mesh); }
};

DELEGATE_BIG3 (PATriangulateQuad)

DELEGATE3 (WingedEdge&, PATriangulateQuad, run, WingedMesh&, WingedFace&, std::unordered_set <WingedFace*>*)
DELEGATE1 (void, PATriangulateQuad, runUndo, WingedMesh&)
DELEGATE1 (void, PATriangulateQuad, runRedo, WingedMesh&)

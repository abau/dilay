#include "partial-action/triangulate-quad.hpp"
#include "winged-edge.hpp"
#include "winged-face.hpp"
#include "winged-mesh.hpp"
#include "action/unit/on-winged-mesh.hpp"
#include "partial-action/modify-winged-mesh.hpp"
#include "partial-action/modify-winged-edge.hpp"
#include "partial-action/modify-winged-face.hpp"
#include "triangle.hpp"

struct PATriangulateQuad :: Impl {
  ActionUnitOnWMesh actions;

  void run (WingedMesh& mesh, WingedFace& face, std::list <Id>* affectedFaces) {
    assert (face.numEdges () == 4);
    this->actions.reset ();

    WingedVertex& vertex      = face.designatedTVertexRef ();
    WingedEdge&   edge        = face.adjacentRef  (vertex);
    WingedEdge&   counterpart = edge.successorRef (face,1);
    WingedFace*   newFace;
    WingedEdge*   newEdge;

    if (edge.isFirstVertex (face, vertex)) {
      Triangle newLeftGeometry (mesh, edge.vertexRef (face, 0)
                                    , edge.vertexRef (face, 2)
                                    , edge.vertexRef (face, 3));

      newFace = &this->actions.add <PAModifyWMesh> ()->addFace (mesh, newLeftGeometry);
      newEdge = &splitFaceWith ( mesh, *newFace, face
                               , edge.predecessorRef (face), counterpart
                               , edge.successorRef (face), edge);
    }
    else {
      Triangle newLeftGeometry (mesh, edge.vertexRef (face, 0)
                                    , edge.vertexRef (face, 1)
                                    , edge.vertexRef (face, 3));

      newFace = &this->actions.add <PAModifyWMesh> ()->addFace (mesh, newLeftGeometry);
      newEdge = &splitFaceWith ( mesh, *newFace ,face
                               , edge       , edge.predecessorRef (face)
                               , counterpart, edge.successorRef   (face));
    }
    this->actions.add <PAModifyWEdge> ()->isTEdge (*newEdge, true);
    this->actions.add <PAModifyWFace> ()->write   (mesh, *newFace);
    this->actions.add <PAModifyWFace> ()->write   (mesh, face);

    if (affectedFaces) {
      affectedFaces->push_back (face.    id ());
      affectedFaces->push_back (newFace->id ());
    }
  }

  WingedEdge& splitFaceWith ( WingedMesh& mesh
                            , WingedFace& newLeft, WingedFace& faceToSplit
                            , WingedEdge& leftPred,  WingedEdge& leftSucc
                            , WingedEdge& rightPred, WingedEdge& rightSucc) {

    WingedEdge& splitAlong = this->actions.add <PAModifyWMesh> ()->
      addEdge (mesh, WingedEdge ());

    this->actions.add <PAModifyWFace> ()->edge (newLeft    , &splitAlong);
    this->actions.add <PAModifyWFace> ()->edge (faceToSplit, &splitAlong);

    this->actions.add <PAModifyWEdge> ()->
      vertex1 (splitAlong, leftPred.secondVertex (faceToSplit));
    this->actions.add <PAModifyWEdge> ()->
      vertex2 (splitAlong, leftSucc.firstVertex (faceToSplit));

    this->actions.add <PAModifyWEdge> ()->leftFace  (splitAlong, &newLeft);
    this->actions.add <PAModifyWEdge> ()->rightFace (splitAlong, &faceToSplit);

    this->actions.add <PAModifyWEdge> ()->leftPredecessor  (splitAlong, &leftPred);
    this->actions.add <PAModifyWEdge> ()->leftSuccessor    (splitAlong, &leftSucc);
    this->actions.add <PAModifyWEdge> ()->rightPredecessor (splitAlong, &rightPred);
    this->actions.add <PAModifyWEdge> ()->rightSuccessor   (splitAlong, &rightSucc);

    this->actions.add <PAModifyWEdge> ()->face        (leftPred, faceToSplit, &newLeft);
    this->actions.add <PAModifyWEdge> ()->predecessor (leftPred, newLeft, &leftSucc);
    this->actions.add <PAModifyWEdge> ()->successor   (leftPred, newLeft, &splitAlong);

    this->actions.add <PAModifyWEdge> ()->face        (leftSucc, faceToSplit, &newLeft);
    this->actions.add <PAModifyWEdge> ()->predecessor (leftSucc, newLeft, &splitAlong);
    this->actions.add <PAModifyWEdge> ()->successor   (leftSucc, newLeft, &leftPred);

    this->actions.add <PAModifyWEdge> ()->predecessor (rightPred, faceToSplit, &rightSucc);
    this->actions.add <PAModifyWEdge> ()->successor   (rightPred, faceToSplit, &splitAlong);

    this->actions.add <PAModifyWEdge> ()->predecessor (rightSucc, faceToSplit, &splitAlong);
    this->actions.add <PAModifyWEdge> ()->successor   (rightSucc, faceToSplit, &rightPred);

    return splitAlong;
  }

  void undo (WingedMesh& mesh) { this->actions.undo (mesh); }
  void redo (WingedMesh& mesh) { this->actions.redo (mesh); }
};

DELEGATE_ACTION_BIG6 (PATriangulateQuad)

DELEGATE3 (void,PATriangulateQuad,run,WingedMesh&,WingedFace&,std::list <Id>*)
DELEGATE1 (void,PATriangulateQuad,undo,WingedMesh&)
DELEGATE1 (void,PATriangulateQuad,redo,WingedMesh&)


#include "partial-action/triangulate-quad.hpp"
#include "macro.hpp"
#include "winged-edge.hpp"
#include "winged-face.hpp"
#include "winged-mesh.hpp"
#include "action/unit.hpp"
#include "partial-action/modify-mesh.hpp"
#include "partial-action/modify-edge.hpp"
#include "partial-action/modify-face.hpp"
#include "triangle.hpp"

struct PATriangulateQuad :: Impl {
  ActionUnit actions;

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

      newFace = &this->actions.add <PAModifyMesh> ()->addFace (mesh, newLeftGeometry);
      newEdge = &splitFaceWith ( mesh, *newFace, face
                               , edge.predecessorRef (face), counterpart
                               , edge.successorRef (face), edge);
    }
    else {
      Triangle newLeftGeometry (mesh, edge.vertexRef (face, 0)
                                    , edge.vertexRef (face, 1)
                                    , edge.vertexRef (face, 3));

      newFace = &this->actions.add <PAModifyMesh> ()->addFace (mesh, newLeftGeometry);
      newEdge = &splitFaceWith ( mesh, *newFace ,face
                               , edge       , edge.predecessorRef (face)
                               , counterpart, edge.successorRef   (face));
    }
    this->actions.add <PAModifyEdge> ()->isTEdge (mesh, *newEdge, true);
    this->actions.add <PAModifyFace> ()->write   (mesh, *newFace);
    this->actions.add <PAModifyFace> ()->write   (mesh, face);

    if (affectedFaces) {
      affectedFaces->push_back (face.    id ());
      affectedFaces->push_back (newFace->id ());
    }
  }

  WingedEdge& splitFaceWith ( WingedMesh& mesh
                            , WingedFace& newLeft, WingedFace& faceToSplit
                            , WingedEdge& leftPred,  WingedEdge& leftSucc
                            , WingedEdge& rightPred, WingedEdge& rightSucc) {

    WingedEdge& splitAlong = this->actions.add <PAModifyMesh> ()->
      addEdge (mesh, WingedEdge ());

    this->actions.add <PAModifyFace> ()->edge (mesh, newLeft    , &splitAlong);
    this->actions.add <PAModifyFace> ()->edge (mesh, faceToSplit, &splitAlong);

    this->actions.add <PAModifyEdge> ()->
      vertex1 (mesh, splitAlong, leftPred.secondVertex (faceToSplit));
    this->actions.add <PAModifyEdge> ()->
      vertex2 (mesh, splitAlong, leftSucc.firstVertex (faceToSplit));

    this->actions.add <PAModifyEdge> ()->leftFace  (mesh, splitAlong, &newLeft);
    this->actions.add <PAModifyEdge> ()->rightFace (mesh, splitAlong, &faceToSplit);

    this->actions.add <PAModifyEdge> ()->leftPredecessor  (mesh, splitAlong, &leftPred);
    this->actions.add <PAModifyEdge> ()->leftSuccessor    (mesh, splitAlong, &leftSucc);
    this->actions.add <PAModifyEdge> ()->rightPredecessor (mesh, splitAlong, &rightPred);
    this->actions.add <PAModifyEdge> ()->rightSuccessor   (mesh, splitAlong, &rightSucc);

    this->actions.add <PAModifyEdge> ()->face        (mesh, leftPred, faceToSplit, &newLeft);
    this->actions.add <PAModifyEdge> ()->predecessor (mesh, leftPred, newLeft, &leftSucc);
    this->actions.add <PAModifyEdge> ()->successor   (mesh, leftPred, newLeft, &splitAlong);

    this->actions.add <PAModifyEdge> ()->face        (mesh, leftSucc, faceToSplit, &newLeft);
    this->actions.add <PAModifyEdge> ()->predecessor (mesh, leftSucc, newLeft, &splitAlong);
    this->actions.add <PAModifyEdge> ()->successor   (mesh, leftSucc, newLeft, &leftPred);

    this->actions.add <PAModifyEdge> ()->predecessor (mesh, rightPred, faceToSplit, &rightSucc);
    this->actions.add <PAModifyEdge> ()->successor   (mesh, rightPred, faceToSplit, &splitAlong);

    this->actions.add <PAModifyEdge> ()->predecessor (mesh, rightSucc, faceToSplit, &splitAlong);
    this->actions.add <PAModifyEdge> ()->successor   (mesh, rightSucc, faceToSplit, &rightPred);

    return splitAlong;
  }


  void undo () { this->actions.undo (); }
  void redo () { this->actions.redo (); }
};

DELEGATE_CONSTRUCTOR (PATriangulateQuad)
DELEGATE_DESTRUCTOR  (PATriangulateQuad)

DELEGATE3 (void,PATriangulateQuad,run,WingedMesh&,WingedFace&,std::list <Id>*)
DELEGATE  (void,PATriangulateQuad,undo)
DELEGATE  (void,PATriangulateQuad,redo)


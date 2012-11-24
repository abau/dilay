#include "adaptive-mesh.hpp"

LinkedFace* splitFaceAlong ( WingedMesh& mesh, LinkedFace& faceToSplit
                           , LinkedEdge& splitAlong
                           , LinkedEdge& leftPred, LinkedEdge& leftSucc
                           , LinkedEdge& rightPred, LinkedEdge& rightSucc) {
  faceToSplit.data ().increaseDepth ();
  LinkedFace* newLeft = mesh.addFace (WingedFace (0,faceToSplit.data ().depth ()));

  newLeft   ->data ().setEdge             (&splitAlong);
  faceToSplit.data ().setEdge             (&splitAlong);

  splitAlong .data ().setVertex1          (leftPred.data ().secondVertex (faceToSplit.data()));
  splitAlong .data ().setVertex2          (leftSucc.data ().firstVertex  (faceToSplit.data()));

  splitAlong .data ().setLeftFace         (newLeft);
  splitAlong .data ().setRightFace        (&faceToSplit);

  splitAlong .data ().setLeftPredecessor  (&leftPred);
  splitAlong .data ().setLeftSuccessor    (&leftSucc);
  splitAlong .data ().setRightPredecessor (&rightPred);
  splitAlong .data ().setRightSuccessor   (&rightSucc);

  leftPred   .data ().setFace             (faceToSplit.data (), newLeft);
  leftPred   .data ().setPredecessor      (newLeft->data ()   , &leftSucc);
  leftPred   .data ().setSuccessor        (newLeft->data ()   , &splitAlong);

  leftSucc   .data ().setFace             (faceToSplit.data (), newLeft);
  leftSucc   .data ().setPredecessor      (newLeft->data ()   , &splitAlong);
  leftSucc   .data ().setSuccessor        (newLeft->data ()   , &leftPred);

  rightPred  .data ().setPredecessor      (faceToSplit.data (), &rightSucc);
  rightPred  .data ().setSuccessor        (faceToSplit.data (), &splitAlong);

  rightSucc  .data ().setPredecessor      (faceToSplit.data (), &splitAlong);
  rightSucc  .data ().setSuccessor        (faceToSplit.data (), &rightPred);

  return newLeft;
}

void AdaptiveMesh :: splitEdge (WingedMesh& mesh, LinkedEdge& edgeToSplit) {
  WingedEdge&   e  = edgeToSplit.data ();
  LinkedVertex* v1 = e.vertex1 ();
  LinkedVertex* v2 = e.vertex2 ();

  LinkedVertex* vNew = mesh.addVertex 
    ( Util :: between ( mesh.vertex (v1->data ().index ())
                      , mesh.vertex (v2->data ().index ()))
    , &edgeToSplit);

  e.setVertex1 (vNew);

  LinkedEdge* eNew   = mesh.addEdge (WingedEdge 
                                        ( v1, vNew
                                        , e.leftFace (), e.rightFace ()
                                        , e.leftPredecessor (), 0
                                        , e.rightPredecessor (), 0));
  if (e.leftFace () != 0) {
    LinkedEdge* eLeft = mesh.addEdge (WingedEdge (0,0,0,0,0,0,0,0));
    splitFaceAlong (mesh, *e.leftFace (), *eLeft, *eNew, *e.leftPredecessor ()
                                                , *e.leftSuccessor (), edgeToSplit ); 
  }
  if (e.rightFace () != 0) {
    LinkedEdge* eRight = mesh.addEdge (WingedEdge (0,0,0,0,0,0,0,0));
    splitFaceAlong (mesh, *e.rightFace (), *eRight, edgeToSplit, *e.rightPredecessor ()
                                                  , *e.rightSuccessor (), *eNew ); 
  }
}

void AdaptiveMesh :: splitFace (WingedMesh& mesh, LinkedFace& face) {
  AdaptiveMesh :: splitEdge (mesh,face.data().longestEdge(mesh));
}

void AdaptiveMesh :: splitFaceRegular (WingedMesh& mesh, LinkedFace& face) {
  LinkedEdge& edge = face.data().longestEdge(mesh);
  AdaptiveMesh :: equalizeFaces (mesh, edge);
  AdaptiveMesh :: splitEdge     (mesh, edge);
}

void AdaptiveMesh :: equalizeFaces (WingedMesh& mesh, LinkedEdge& linkedEdge) {
  WingedEdge& edge    = linkedEdge.data ();

  if (  edge.leftFace () == 0 || edge.rightFace () == 0
     || edge.leftFace ()->data ().depth () == edge.rightFace ()->data ().depth ())
    return;
  else if (edge.leftFace ()->data ().depth () < edge.rightFace ()->data ().depth ()) 
    splitFaceRegular (mesh, *edge.leftFace ());
  else 
    splitFaceRegular (mesh, *edge.rightFace ());

  equalizeFaces (mesh, linkedEdge);
}

#include "subdivision-util.hpp"

LinkedFace* splitFaceWith ( WingedMesh& mesh, LinkedFace& faceToSplit
                          , LinkedEdge& leftPred, LinkedEdge& leftSucc
                          , LinkedEdge& rightPred, LinkedEdge& rightSucc) {

  LinkedEdge* splitAlong = mesh.addEdge (WingedEdge (0,0,0,0,0,0,0,0));
  LinkedFace* newLeft    = mesh.addFace (WingedFace (0));

  newLeft   ->data ().setEdge             (splitAlong);
  faceToSplit.data ().setEdge             (splitAlong);

  splitAlong->data ().setVertex1          (leftPred.data ().secondVertex (faceToSplit.data()));
  splitAlong->data ().setVertex2          (leftSucc.data ().firstVertex  (faceToSplit.data()));

  splitAlong->data ().setLeftFace         (newLeft);
  splitAlong->data ().setRightFace        (&faceToSplit);

  splitAlong->data ().setLeftPredecessor  (&leftPred);
  splitAlong->data ().setLeftSuccessor    (&leftSucc);
  splitAlong->data ().setRightPredecessor (&rightPred);
  splitAlong->data ().setRightSuccessor   (&rightSucc);

  leftPred   .data ().setFace             (faceToSplit.data (), newLeft);
  leftPred   .data ().setPredecessor      (newLeft->data ()   , &leftSucc);
  leftPred   .data ().setSuccessor        (newLeft->data ()   , splitAlong);

  leftSucc   .data ().setFace             (faceToSplit.data (), newLeft);
  leftSucc   .data ().setPredecessor      (newLeft->data ()   , splitAlong);
  leftSucc   .data ().setSuccessor        (newLeft->data ()   , &leftPred);

  rightPred  .data ().setPredecessor      (faceToSplit.data (), &rightSucc);
  rightPred  .data ().setSuccessor        (faceToSplit.data (), splitAlong);

  rightSucc  .data ().setPredecessor      (faceToSplit.data (), splitAlong);
  rightSucc  .data ().setSuccessor        (faceToSplit.data (), &rightPred);

  return newLeft;
}

NewFaces SubdivUtil :: splitEdge ( WingedMesh& mesh, LinkedEdge& edgeToSplit
                                 , const glm::vec3& vNew) {
  WingedEdge&   e         = edgeToSplit.data ();
  LinkedVertex* v1        = edgeToSplit.data ().vertex1 ();
  LinkedVertex* linkedNew = mesh.addVertex (vNew, &edgeToSplit);

  e.setVertex1 (linkedNew);
  LinkedEdge* eNew = mesh.addEdge (WingedEdge 
                                    ( v1, linkedNew
                                    , e.leftFace (), e.rightFace ()
                                    , e.leftPredecessor (), 0, 0, 0));
  v1->data ().setEdge (eNew);

  LinkedFace* f1New = 0;
  LinkedFace* f2New = 0;

  if (e.leftFace () != 0) {
    f1New = splitFaceWith (mesh, *e.leftFace (), *eNew, *e.leftPredecessor ()
                                               , *e.leftSuccessor (), edgeToSplit); 
  }
  if (e.rightFace () != 0) {
    f2New = splitFaceWith (mesh, *e.rightFace (), edgeToSplit, *e.rightPredecessor ()
                                                , *e.rightSuccessor (), *eNew ); 
  }
  return std::pair <LinkedFace*,LinkedFace*> (f1New,f2New);
}

NewFaces SubdivUtil :: splitEdge ( WingedMesh& mesh, LinkedEdge& edgeToSplit) {
  return SubdivUtil :: splitEdge (mesh, edgeToSplit, edgeToSplit.data ().middle (mesh));
}

LinkedEdge* SubdivUtil :: insertVertex ( WingedMesh& mesh, LinkedEdge& linkedEdge
                                       , const glm::vec3& v) {
  WingedEdge&   e    = linkedEdge.data ();
  LinkedVertex* newV = mesh.addVertex (v, &linkedEdge);
  LinkedEdge*   newE = mesh.addEdge (WingedEdge 
                                      ( newV, e.vertex2 ()
                                      , e.leftFace ()        , e.rightFace ()
                                      , &linkedEdge          , e.leftSuccessor ()
                                      , e.rightPredecessor (), &linkedEdge));

  e.vertex2 ()->data ().setEdge (newE);
  e.setVertex2 (newV);
  return newE;
}

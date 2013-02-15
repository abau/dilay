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

void SubdivUtil :: splitEdge ( WingedMesh& mesh, LinkedEdge& edgeToSplit
                               , const glm::vec3& vNew) {
  WingedEdge&   e         = edgeToSplit.data ();
  LinkedVertex* v1        = edgeToSplit.data ().vertex1 ();
  LinkedVertex* linkedNew = mesh.addVertex (vNew, &edgeToSplit);

  e.setVertex1 (linkedNew);
  LinkedEdge* eNew   = mesh.addEdge (WingedEdge 
                                        ( v1, linkedNew
                                        , e.leftFace (), e.rightFace ()
                                        , e.leftPredecessor (), 0, 0, 0));
  v1->data ().setEdge (eNew);

  if (e.leftFace () != 0) {
    splitFaceWith (mesh, *e.leftFace (), *eNew              , *e.leftPredecessor ()
                                       , *e.leftSuccessor (), edgeToSplit ); 
  }
  if (e.rightFace () != 0) {
    splitFaceWith (mesh, *e.rightFace (), edgeToSplit         , *e.rightPredecessor ()
                                        , *e.rightSuccessor (), *eNew ); 
  }
}

void SubdivUtil :: splitEdge ( WingedMesh& mesh, LinkedEdge& edgeToSplit) {
  SubdivUtil :: splitEdge ( mesh, edgeToSplit, edgeToSplit.data ().middle (mesh));
}

void SubdivUtil :: splitFaceRegular (WingedMesh& mesh, LinkedFace& face) {
  LinkedEdge& edge = face.data().longestEdge(mesh);
  SubdivUtil :: equalizeFaces (mesh, face, edge);
  SubdivUtil :: splitEdge     (mesh, edge);
}

void SubdivUtil :: equalizeFaces ( WingedMesh& mesh
                                   , LinkedFace& linkedFace
                                   , LinkedEdge& linkedEdge
                                   ) {
  WingedEdge& edge      = linkedEdge.data ();
  WingedFace& face      = linkedFace.data ();

  LinkedFace* otherLinkedFace = edge.otherFace (face);
  LinkedEdge& otherLinkedEdge = otherLinkedFace->data ().longestEdge (mesh);

  if (&linkedEdge == &otherLinkedEdge)
    return;
  splitFaceRegular (mesh, *otherLinkedFace);
  equalizeFaces    (mesh, linkedFace, linkedEdge);
}

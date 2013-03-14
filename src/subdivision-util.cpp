#include <cassert>
#include "subdivision-util.hpp"
#include "winged-vertex.hpp"
#include "winged-edge.hpp"
#include "winged-face.hpp"
#include "winged-mesh.hpp"

LinkedFace splitFaceWith ( WingedMesh& mesh, LinkedFace faceToSplit
                         , LinkedEdge leftPred,  LinkedEdge leftSucc
                         , LinkedEdge rightPred, LinkedEdge rightSucc) {

  LinkedEdge splitAlong = mesh.addEdge (WingedEdge (mesh));
  LinkedFace newLeft    = mesh.addFace (WingedFace (mesh.nullEdge ()));

  newLeft->setEdge                (splitAlong);
  faceToSplit->setEdge            (splitAlong);

  splitAlong->setVertex1          (leftPred->secondVertex (*faceToSplit));
  splitAlong->setVertex2          (leftSucc->firstVertex  (*faceToSplit));

  splitAlong->setLeftFace         (newLeft);
  splitAlong->setRightFace        (faceToSplit);

  splitAlong->setLeftPredecessor  (leftPred);
  splitAlong->setLeftSuccessor    (leftSucc);
  splitAlong->setRightPredecessor (rightPred);
  splitAlong->setRightSuccessor   (rightSucc);

  leftPred->setFace               (*faceToSplit, newLeft);
  leftPred->setPredecessor        (*newLeft, leftSucc);
  leftPred->setSuccessor          (*newLeft, splitAlong);

  leftSucc->setFace               (*faceToSplit, newLeft);
  leftSucc->setPredecessor        (*newLeft, splitAlong);
  leftSucc->setSuccessor          (*newLeft, leftPred);

  rightPred->setPredecessor       (*faceToSplit, rightSucc);
  rightPred->setSuccessor         (*faceToSplit, splitAlong);

  rightSucc->setPredecessor       (*faceToSplit, splitAlong);
  rightSucc->setSuccessor         (*faceToSplit, rightPred);

  return newLeft;
}

/*
NewFaces SubdivUtil :: splitEdge ( WingedMesh& mesh, LinkedEdge e
                                 , const glm::vec3& vNew) {
  LinkedVertex v1        = e->vertex1 ();
  LinkedVertex linkedNew = mesh.addVertex (vNew, e);
  LinkedEdge   nullE     = mesh.nullEdge ();

  e->setVertex1 (linkedNew);
  LinkedEdge eNew = mesh.addEdge (WingedEdge 
                                   ( v1, linkedNew
                                   , e->leftFace (), e->rightFace ()
                                   , e->leftPredecessor (), nullE, nullE, nullE));
  v1->setEdge (eNew);

  LinkedFace f1New;
  LinkedFace f2New;

  if (e->leftFace () != mesh.nullFace ()) {
    f1New = splitFaceWith (mesh, e->leftFace (), eNew, e->leftPredecessor ()
                                               , e->leftSuccessor (), e); 
  }
  if (e->rightFace () != mesh.nullFace ()) {
    f2New = splitFaceWith (mesh, e->rightFace (), e, e->rightPredecessor ()
                                                , e->rightSuccessor (), eNew ); 
  }
  return NewFaces (f1New,f2New);
}

NewFaces SubdivUtil :: splitEdge (WingedMesh& mesh, LinkedEdge edge) {
  return SubdivUtil :: splitEdge (mesh, edge, edge->middle (mesh));
}
*/

LinkedEdge SubdivUtil :: insertVertex ( WingedMesh& mesh, LinkedEdge e
                                      , const glm::vec3& v) {
  //   newE        e
  // 1----->newV------->2
  LinkedVertex newV = mesh.addVertex (v, e);
  LinkedEdge   newE = mesh.addEdge (WingedEdge 
                                      ( e->vertex1 (), newV
                                      , e->leftFace ()        , e->rightFace ()
                                      , e->leftPredecessor () , e
                                      , e                     , e->rightSuccessor ()));
  e->setVertex1         (newV);
  e->setSuccessor       (*e->rightFace (), newE);
  e->setPredecessor     (*e->leftFace (),  newE);
  newE->leftPredecessor ()->setSuccessor   (*newE->leftFace (), newE);
  newE->rightSuccessor  ()->setPredecessor (*newE->rightFace (), newE);
  newE->vertex1   ()->setEdge (newE);
  newE->leftFace  ()->setEdge (newE);
  return newE;
}

void SubdivUtil :: triangulate6Gon (WingedMesh& mesh, LinkedFace f) {
  assert (f->numEdges () == 6);

  /*     4
   *    /c\
   *   5---3
   *  /a\f/b\
   * 0---1---2
   */
  LinkedEdge e0   = f->edge ();
  LinkedEdge e1   = f->edge ()->successor (*f,0);
  LinkedEdge e2   = f->edge ()->successor (*f,1);
  LinkedEdge e3   = f->edge ()->successor (*f,2);
  LinkedEdge e4   = f->edge ()->successor (*f,3);
  LinkedEdge e5   = f->edge ()->successor (*f,4);

  LinkedVertex v1 = e1->firstVertex (*f);
  LinkedVertex v3 = e3->firstVertex (*f);
  LinkedVertex v5 = e5->firstVertex (*f);

  LinkedFace a = mesh.addFace (WingedFace (e0));
  LinkedFace b = mesh.addFace (WingedFace (e2));
  LinkedFace c = mesh.addFace (WingedFace (e4));

  LinkedEdge e13 = mesh.addEdge (WingedEdge (mesh));
  LinkedEdge e35 = mesh.addEdge (WingedEdge (mesh));
  LinkedEdge e51 = mesh.addEdge (WingedEdge (mesh));

  e13->set (v1,v3,f,b,e51,e35,e2,e1);
  e35->set (v3,v5,f,c,e13,e51,e4,e3);
  e51->set (v5,v1,f,a,e35,e13,e0,e5);

  e0->setFace         (*f,a);
  e0->setPredecessor  (*a,e5);
  e0->setSuccessor    (*a,e51);

  e1->setFace         (*f,b);
  e1->setPredecessor  (*b,e13);
  e1->setSuccessor    (*b,e2);

  e2->setFace         (*f,b);
  e2->setPredecessor  (*b,e1);
  e2->setSuccessor    (*b,e13);

  e3->setFace         (*f,c);
  e3->setPredecessor  (*c,e35);
  e3->setSuccessor    (*c,e4);

  e4->setFace         (*f,c);
  e4->setPredecessor  (*c,e3);
  e4->setSuccessor    (*c,e35);

  e5->setFace         (*f,a);
  e5->setPredecessor  (*a,e51);
  e5->setSuccessor    (*a,e0);

  f->setEdge (e13);
}

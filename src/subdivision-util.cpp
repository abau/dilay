#include <cassert>
#include <algorithm>
#include "subdivision-util.hpp"
#include "winged-vertex.hpp"
#include "winged-edge.hpp"
#include "winged-face.hpp"
#include "winged-mesh.hpp"
#include "winged-util.hpp"
#include "adjacent-iterator.hpp"
#include "maybe.hpp"

LinkedEdge splitFaceWith ( WingedMesh&, LinkedFace, LinkedEdge, LinkedEdge
                         , LinkedEdge, LinkedEdge);

LinkedEdge SubdivUtil :: insertVertex ( WingedMesh& mesh, LinkedEdge e
                                      , const glm::vec3& v) {
  //   newE        e
  // 1----->newV------->2
  unsigned int level = 1 + std::max <unsigned int> ( e->vertex1 ()->level ()
                                                   , e->vertex2 ()->level () );
  LinkedVertex newV  = mesh.addVertex (v,level);
  LinkedEdge   newE  = mesh.addEdge (WingedEdge 
                                      ( e->vertex1 (), newV
                                      , e->leftFace ()        , e->rightFace ()
                                      , e->leftPredecessor () , e
                                      , e                     , e->rightSuccessor ()
                                      , e->previousSibling () , Maybe <LinkedEdge> (e)
                                      ));
  e->setVertex1         (newV);
  e->setSuccessor       (*e->rightFace (), newE);
  e->setPredecessor     (*e->leftFace (),  newE);
  e->setPreviousSibling (newE);
  newV->setEdge         (e);
  newE->leftPredecessor ()->setSuccessor   (*newE->leftFace (), newE);
  newE->rightSuccessor  ()->setPredecessor (*newE->rightFace (), newE);
  newE->vertex1         ()->setEdge (newE);
  newE->leftFace        ()->setEdge (newE);
  if (newE->previousSibling ().isDefined ())
    newE->previousSibling ().data ()->setNextSibling (newE);
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
  LinkedEdge e01  = f->edge ();
  LinkedEdge e12  = f->edge ()->successor (*f,0);
  LinkedEdge e23  = f->edge ()->successor (*f,1);
  LinkedEdge e34  = f->edge ()->successor (*f,2);
  LinkedEdge e45  = f->edge ()->successor (*f,3);
  LinkedEdge e50  = f->edge ()->successor (*f,4);

  LinkedVertex v1 = e12->firstVertex (*f);
  LinkedVertex v3 = e34->firstVertex (*f);
  LinkedVertex v5 = e50->firstVertex (*f);

  LinkedFace a = mesh.addFace (WingedFace (e01));
  LinkedFace b = mesh.addFace (WingedFace (e23));
  LinkedFace c = mesh.addFace (WingedFace (e45));

  LinkedEdge e13 = mesh.addEdge (WingedEdge ());
  LinkedEdge e35 = mesh.addEdge (WingedEdge ());
  LinkedEdge e51 = mesh.addEdge (WingedEdge ());

  e13->setGeometry (v1,v3,f,b,e51,e35,e23,e12);
  e35->setGeometry (v3,v5,f,c,e13,e51,e45,e34);
  e51->setGeometry (v5,v1,f,a,e35,e13,e01,e50);

  e01->setFace         (*f,a);
  e01->setPredecessor  (*a,e50);
  e01->setSuccessor    (*a,e51);

  e12->setFace         (*f,b);
  e12->setPredecessor  (*b,e13);
  e12->setSuccessor    (*b,e23);

  e23->setFace         (*f,b);
  e23->setPredecessor  (*b,e12);
  e23->setSuccessor    (*b,e13);

  e34->setFace         (*f,c);
  e34->setPredecessor  (*c,e35);
  e34->setSuccessor    (*c,e45);

  e45->setFace         (*f,c);
  e45->setPredecessor  (*c,e34);
  e45->setSuccessor    (*c,e35);

  e50->setFace         (*f,a);
  e50->setPredecessor  (*a,e51);
  e50->setSuccessor    (*a,e01);

  f->setEdge          (e13);
}

void SubdivUtil :: triangulateQuadAtHeighestLevelVertex ( WingedMesh& mesh
                                                        , LinkedFace face) {
  assert (face->numEdges () == 4);

  LinkedVertex vertex      = face->highestLevelVertex ();
  LinkedEdge   edge        = face->adjacent           (*vertex);
  LinkedEdge   counterpart = edge->successor          (*face,1);
  LinkedEdge   newEdge;

  if (edge->isFirstVertex (*face, *vertex)) {
    newEdge = splitFaceWith ( mesh, face
                            , edge->predecessor (*face), counterpart
                            , edge->successor   (*face), edge);
  }
  else {
    newEdge = splitFaceWith ( mesh, face
                            , edge       , edge->predecessor (*face)
                            , counterpart, edge->successor   (*face));
  }
  newEdge->isTEdge (true);
}

// Internal /////////////

LinkedEdge splitFaceWith ( WingedMesh& mesh, LinkedFace faceToSplit
                         , LinkedEdge leftPred,  LinkedEdge leftSucc
                         , LinkedEdge rightPred, LinkedEdge rightSucc) {

  LinkedEdge splitAlong = mesh.addEdge (WingedEdge ());
  LinkedFace newLeft    = mesh.addFace (WingedFace (splitAlong));

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

  return splitAlong;
}


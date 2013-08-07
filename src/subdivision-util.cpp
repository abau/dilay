#include <cassert>
#include <algorithm>
#include "subdivision-util.hpp"
#include "winged-vertex.hpp"
#include "winged-edge.hpp"
#include "winged-face.hpp"
#include "winged-mesh.hpp"
#include "winged-util.hpp"
#include "adjacent-iterator.hpp"
#include "triangle.hpp"

WingedEdge& SubdivUtil :: insertVertex ( WingedMesh& mesh, WingedEdge& e
                                       , const glm::vec3& v) {
  //   newE        e
  // 1----->newV------->2
  unsigned int level = 1 + std::max <unsigned int> ( e.vertex1Ref ().level ()
                                                   , e.vertex2Ref ().level () );
  WingedVertex& newV  = mesh.addVertex (v,level);
  WingedEdge&   newE  = mesh.addEdge (WingedEdge 
                                      ( e.vertex1 ()         , &newV
                                      , e.leftFace ()        , e.rightFace ()
                                      , e.leftPredecessor () , &e
                                      , &e                   , e.rightSuccessor ()
                                      , e.previousSibling () , &e)
                                      );
  e.vertex1         (&newV);
  e.successor       (e.rightFaceRef (), &newE);
  e.predecessor     (e.leftFaceRef (),  &newE);
  e.previousSibling (&newE);
  newV.edge         (&e);
  newE.leftPredecessorRef ().successor   (newE.leftFaceRef  (), &newE);
  newE.rightSuccessorRef  ().predecessor (newE.rightFaceRef (), &newE);
  newE.vertex1Ref         ().edge (&newE);
  newE.leftFaceRef        ().edge (&newE);
  if (newE.previousSibling ())
    newE.previousSiblingRef ().nextSibling (&newE);
  return newE;
}

WingedFace& SubdivUtil :: triangulate6Gon (WingedMesh& mesh, WingedFace& f) {
  assert (f.numEdges () == 6);

  /*     4
   *    /c\
   *   5---3
   *  /a\f/b\
   * 0---1---2
   */
  WingedEdge& e01  = f.edgeRef ();
  WingedEdge& e12  = f.edgeRef ().successorRef (f,0);
  WingedEdge& e23  = f.edgeRef ().successorRef (f,1);
  WingedEdge& e34  = f.edgeRef ().successorRef (f,2);
  WingedEdge& e45  = f.edgeRef ().successorRef (f,3);
  WingedEdge& e50  = f.edgeRef ().successorRef (f,4);

  WingedVertex& v0 = e01.firstVertexRef (f);
  WingedVertex& v1 = e12.firstVertexRef (f);
  WingedVertex& v2 = e23.firstVertexRef (f);
  WingedVertex& v3 = e34.firstVertexRef (f);
  WingedVertex& v4 = e45.firstVertexRef (f);
  WingedVertex& v5 = e50.firstVertexRef (f);

  WingedFace& a = mesh.addFace (WingedFace (&e01), Triangle (mesh,v0,v1,v5));
  WingedFace& b = mesh.addFace (WingedFace (&e23), Triangle (mesh,v1,v2,v3));
  WingedFace& c = mesh.addFace (WingedFace (&e45), Triangle (mesh,v3,v4,v5));

  WingedEdge& e13 = mesh.addEdge (WingedEdge ());
  WingedEdge& e35 = mesh.addEdge (WingedEdge ());
  WingedEdge& e51 = mesh.addEdge (WingedEdge ());

  e13.setGeometry (&v1,&v3,&f,&b,&e51,&e35,&e23,&e12);
  e35.setGeometry (&v3,&v5,&f,&c,&e13,&e51,&e45,&e34);
  e51.setGeometry (&v5,&v1,&f,&a,&e35,&e13,&e01,&e50);

  e01.face         (f,&a);
  e01.predecessor  (a,&e50);
  e01.successor    (a,&e51);

  e12.face         (f,&b);
  e12.predecessor  (b,&e13);
  e12.successor    (b,&e23);

  e23.face         (f,&b);
  e23.predecessor  (b,&e12);
  e23.successor    (b,&e13);

  e34.face         (f,&c);
  e34.predecessor  (c,&e35);
  e34.successor    (c,&e45);

  e45.face         (f,&c);
  e45.predecessor  (c,&e34);
  e45.successor    (c,&e35);

  e50.face         (f,&a);
  e50.predecessor  (a,&e51);
  e50.successor    (a,&e01);

  f.edge           (&e13);

  f.write          (mesh);
  a.write          (mesh);
  b.write          (mesh);
  c.write          (mesh);

  return mesh.realignInOctree (f);
}

WingedEdge& splitFaceWith ( WingedMesh&, WingedFace&, WingedFace&
                          , WingedEdge&, WingedEdge&, WingedEdge&, WingedEdge&);

void SubdivUtil :: triangulateQuadAtHeighestLevelVertex ( WingedMesh& mesh
                                                        , WingedFace& face) {
  assert (face.numEdges () == 4);

  WingedVertex& vertex     = face.highestLevelVertexRef ();
  WingedEdge&  edge        = face.adjacentRef        (vertex);
  WingedEdge&  counterpart = edge.successorRef       (face,1);
  WingedFace*  newFace;
  WingedEdge*  newEdge;

  if (edge.isFirstVertex (face, vertex)) {
    Triangle newLeftGeometry (mesh, edge.vertexRef (face, 0)
                                  , edge.vertexRef (face, 2)
                                  , edge.vertexRef (face, 3));

    newFace = &mesh.addFace (WingedFace (), newLeftGeometry);
    newEdge = &splitFaceWith ( mesh, *newFace, face
                             , edge.predecessorRef (face), counterpart
                             , edge.successorRef (face), edge);
  }
  else {
    Triangle newLeftGeometry (mesh, edge.vertexRef (face, 0)
                                  , edge.vertexRef (face, 1)
                                  , edge.vertexRef (face, 3));

    newFace = &mesh.addFace (WingedFace (), newLeftGeometry);
    newEdge = &splitFaceWith ( mesh, *newFace ,face
                             , edge       , edge.predecessorRef (face)
                             , counterpart, edge.successorRef   (face));
  }
  newEdge->isTEdge (true);
  newFace->write   (mesh);
  face.write       (mesh);
}

// Internal /////////////

WingedEdge& splitFaceWith ( WingedMesh& mesh
                          , WingedFace& newLeft, WingedFace& faceToSplit
                          , WingedEdge& leftPred,  WingedEdge& leftSucc
                          , WingedEdge& rightPred, WingedEdge& rightSucc) {

  WingedEdge& splitAlong = mesh.addEdge (WingedEdge ());

  newLeft.edge                (&splitAlong);
  faceToSplit.edge            (&splitAlong);

  splitAlong.vertex1          (&leftPred.secondVertexRef (faceToSplit));
  splitAlong.vertex2          (&leftSucc.firstVertexRef  (faceToSplit));

  splitAlong.leftFace         (&newLeft);
  splitAlong.rightFace        (&faceToSplit);

  splitAlong.leftPredecessor  (&leftPred);
  splitAlong.leftSuccessor    (&leftSucc);
  splitAlong.rightPredecessor (&rightPred);
  splitAlong.rightSuccessor   (&rightSucc);

  leftPred.face               (faceToSplit, &newLeft);
  leftPred.predecessor        (newLeft, &leftSucc);
  leftPred.successor          (newLeft, &splitAlong);

  leftSucc.face               (faceToSplit, &newLeft);
  leftSucc.predecessor        (newLeft, &splitAlong);
  leftSucc.successor          (newLeft, &leftPred);

  rightPred.predecessor       (faceToSplit, &rightSucc);
  rightPred.successor         (faceToSplit, &splitAlong);

  rightSucc.predecessor       (faceToSplit, &splitAlong);
  rightSucc.successor         (faceToSplit, &rightPred);

  return splitAlong;
}


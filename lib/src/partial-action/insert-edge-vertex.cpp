#include <glm/glm.hpp>
#include "partial-action/insert-edge-vertex.hpp"
#include "winged/edge.hpp"
#include "winged/face.hpp"
#include "winged/mesh.hpp"
#include "winged/vertex.hpp"

WingedEdge& PartialAction :: insertEdgeVertex (WingedMesh& mesh, WingedEdge& e, const glm::vec3& v) {
  //   newE        e
  // 1----->newV------->2

  WingedVertex& newV  = mesh.addVertex (v);
  WingedEdge&   newE  = mesh.addEdge   ();

  newE.vertex1          (e.vertex1 ());
  newE.vertex2          (&newV);
  newE.leftFace         (e.leftFace ());
  newE.rightFace        (e.rightFace ());
  newE.leftPredecessor  (e.leftPredecessor ());
  newE.leftSuccessor    (&e);
  newE.rightPredecessor (&e);
  newE.rightSuccessor   (e.rightSuccessor ());

  e.vertex1     (&newV);
  e.successor   (e.rightFaceRef (), &newE);
  e.predecessor (e.leftFaceRef  (), &newE);

  newV.edge (&e);

  newE.leftPredecessorRef ().successor   (newE.leftFaceRef  (), &newE);
  newE.rightSuccessorRef  ().predecessor (newE.rightFaceRef (), &newE);

  newE.vertex1Ref ().edge (&newE);
  
  newE.leftFaceRef ().edge (&newE);
  e.rightFaceRef   ().edge (&e);

  return newE;
}

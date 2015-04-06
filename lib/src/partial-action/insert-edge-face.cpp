#include "partial-action/insert-edge-face.hpp"
#include "primitive/triangle.hpp"
#include "winged/edge.hpp"
#include "winged/face.hpp"
#include "winged/mesh.hpp"
#include "winged/vertex.hpp"

namespace {
  WingedEdge& splitFaceWith ( WingedMesh& mesh
                            , const PrimTriangle& newLeftGeometry, WingedFace& faceToSplit
                            , WingedEdge& leftPred,  WingedEdge& leftSucc
                            , WingedEdge& rightPred, WingedEdge& rightSucc )
  {
    WingedFace& newLeft    = mesh.addFace (newLeftGeometry);
    WingedEdge& splitAlong = mesh.addEdge ();

    newLeft    .edge (&splitAlong);
    faceToSplit.edge (&splitAlong);

    splitAlong.vertex1          (leftPred.secondVertex (faceToSplit));
    splitAlong.vertex2          (leftSucc.firstVertex  (faceToSplit));
    splitAlong.leftFace         (&newLeft);
    splitAlong.rightFace        (&faceToSplit);
    splitAlong.leftPredecessor  (&leftPred);
    splitAlong.leftSuccessor    (&leftSucc);
    splitAlong.rightPredecessor (&rightPred);
    splitAlong.rightSuccessor   (&rightSucc);

    leftPred.face         (faceToSplit, &newLeft);
    leftPred.successor    (newLeft, &splitAlong);

    leftSucc.face         (faceToSplit, &newLeft);
    leftSucc.predecessor  (newLeft, &splitAlong);

    rightPred.successor   (faceToSplit, &splitAlong);
    rightSucc.predecessor (faceToSplit, &splitAlong);

    return splitAlong;
  }
}

WingedEdge& PartialAction :: insertEdgeFace ( WingedMesh& mesh, WingedFace& face
                                            , WingedEdge& edge, WingedVertex& vertex )
{
  assert (edge.isLeftFace (face) || edge.isRightFace (face));

  WingedEdge& predecessor    = edge.predecessorRef (face);
  WingedEdge& prePredecessor = predecessor.predecessorRef (face);
  WingedEdge& successor      = edge.successorRef   (face);
  WingedEdge& sucSuccessor   = successor.successorRef (face);

  if (predecessor.firstVertexRef (face).index () == vertex.index ()) {
    PrimTriangle newLeftGeometry (mesh, predecessor.vertexRef (face, 0)
                                      , predecessor.vertexRef (face, 1)
                                      , predecessor.vertexRef (face, 2));

    return splitFaceWith ( mesh, newLeftGeometry, face
                         , edge, predecessor
                         , prePredecessor, successor );
  }
  else if (sucSuccessor.secondVertexRef (face).index () == vertex.index ()) {
    PrimTriangle newLeftGeometry (mesh, successor.vertexRef (face, 0)
                                      , successor.vertexRef (face, 1)
                                      , successor.vertexRef (face, 2));

    return splitFaceWith ( mesh, newLeftGeometry, face
                         , edge, sucSuccessor.successorRef (face)
                         , sucSuccessor, successor );
  }
  else {
    std::abort ();
  }
}

WingedEdge& PartialAction :: insertEdgeFace (WingedMesh& mesh, WingedFace& face) {
  return PartialAction::insertEdgeFace 
    ( mesh, face, face.edgeRef ()
    , face.edgeRef ().predecessorRef (face).firstVertexRef (face) );
}

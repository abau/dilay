#include "partial-action/insert-edge-face.hpp"
#include "primitive/triangle.hpp"
#include "winged/edge.hpp"
#include "winged/face.hpp"
#include "winged/mesh.hpp"

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

    faceToSplit.writeIndices (mesh);
    newLeft    .writeIndices (mesh);

    return splitAlong;
  }
}

WingedEdge& PartialAction :: insertEdgeFace (WingedMesh& mesh, WingedFace& face) {
  WingedEdge& edge        = face.edgeRef ();
  WingedEdge& predecessor = edge.predecessorRef (face);

  PrimTriangle newLeftGeometry (mesh, predecessor.vertexRef (face, 0)
                                    , predecessor.vertexRef (face, 1)
                                    , predecessor.vertexRef (face, 2));

  return splitFaceWith ( mesh, newLeftGeometry, face
                       , edge, predecessor
                       , predecessor.predecessorRef (face), edge.successorRef (face) );
}

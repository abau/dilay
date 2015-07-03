/* This file is part of Dilay
 * Copyright © 2015 Alexander Bau
 * Use and redistribute under the terms of the GNU General Public License
 */
#include "partial-action/insert-edge-face.hpp"
#include "primitive/triangle.hpp"
#include "winged/edge.hpp"
#include "winged/face.hpp"
#include "winged/mesh.hpp"
#include "winged/vertex.hpp"
#include "util.hpp"

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

WingedEdge& PartialAction :: insertEdgeFace (WingedMesh& mesh, WingedFace& face, WingedEdge& edge) {
  assert (edge.isLeftFace (face) || edge.isRightFace (face));

  WingedEdge&   predecessor    = edge.predecessorRef (face);
  WingedEdge&   prePredecessor = predecessor.predecessorRef (face);
  WingedEdge&   successor      = edge.successorRef   (face);
  WingedEdge&   sucSuccessor   = successor.successorRef (face);
  WingedVertex& vertex         = predecessor.firstVertexRef (face);

  if (predecessor.firstVertexRef (face) == vertex) {
    PrimTriangle newLeftGeometry (mesh, predecessor.vertexRef (face, 0)
                                      , predecessor.vertexRef (face, 1)
                                      , predecessor.vertexRef (face, 2));

    return splitFaceWith ( mesh, newLeftGeometry, face
                         , edge, predecessor
                         , prePredecessor, successor );
  }
  else if (sucSuccessor.secondVertexRef (face) == vertex) {
    PrimTriangle newLeftGeometry (mesh, successor.vertexRef (face, 0)
                                      , successor.vertexRef (face, 1)
                                      , successor.vertexRef (face, 2));

    return splitFaceWith ( mesh, newLeftGeometry, face
                         , edge, sucSuccessor.successorRef (face)
                         , sucSuccessor, successor );
  }
  else {
    DILAY_IMPOSSIBLE
  }
}

WingedEdge& PartialAction :: insertEdgeFace (WingedMesh& mesh, WingedFace& face) {
  return PartialAction::insertEdgeFace (mesh, face, face.edgeRef ());
}

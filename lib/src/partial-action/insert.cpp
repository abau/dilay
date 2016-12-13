/* This file is part of Dilay
 * Copyright © 2015,2016 Alexander Bau
 * Use and redistribute under the terms of the GNU General Public License
 */
#include "affected-faces.hpp"
#include "partial-action/insert.hpp"
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
                            , WingedEdge& rightPred, WingedEdge& rightSucc 
                            , AffectedFaces& affected )
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

    affected.insert (newLeft);
    affected.insert (faceToSplit);

    assert (newLeft.isTriangle ());

    return splitAlong;
  }
}

namespace PartialAction {

  WingedEdge& insertEdgeVertex ( WingedMesh& mesh, WingedEdge& e
                               , const glm::vec3& v, AffectedFaces& affected )
  {
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

    affected.insert (e.leftFaceRef  ());
    affected.insert (e.rightFaceRef ());

    return newE;
  }

  WingedEdge& insertEdgeFace ( WingedMesh& mesh, WingedFace& face, WingedEdge& edge
                             , WingedVertex& vertex, AffectedFaces& affected )
  {
    assert (edge.isLeftFace (face) || edge.isRightFace (face));
    assert (edge.isVertex1 (vertex) || edge.isVertex2 (vertex));

    WingedEdge& predecessor       = edge.predecessorRef (face);
    WingedEdge& prePredecessor    = predecessor.predecessorRef (face);
    WingedEdge& prePrePredecessor = prePredecessor.predecessorRef (face);

    if (edge.firstVertexRef (face) == vertex) {
      PrimTriangle newLeftGeometry (mesh, prePredecessor.vertexRef (face, 0)
                                        , prePredecessor.vertexRef (face, 1)
                                        , prePredecessor.vertexRef (face, 2));

      return splitFaceWith ( mesh, newLeftGeometry, face
                           , predecessor, prePredecessor
                           , prePrePredecessor, edge, affected );
    }
    else if (edge.secondVertexRef (face) == vertex) {
      PrimTriangle newLeftGeometry (mesh, predecessor.vertexRef (face, 0)
                                        , predecessor.vertexRef (face, 1)
                                        , predecessor.vertexRef (face, 2));

      return splitFaceWith ( mesh, newLeftGeometry, face
                           , edge, predecessor
                           , prePredecessor, prePrePredecessor, affected );
    }
    else {
      DILAY_IMPOSSIBLE
    }
  }

  WingedEdge& insertEdgeFaceAtNewVertex ( WingedMesh& mesh, WingedFace& face, WingedEdge& edge
                                        , AffectedFaces& affected )
  {
    if (edge.vertex1Ref ().isNewVertex (mesh)) {
      return insertEdgeFace (mesh, face, edge, edge.vertex1Ref (), affected);
    }
    else if (edge.vertex2Ref ().isNewVertex (mesh)) {
      return insertEdgeFace (mesh, face, edge, edge.vertex2Ref (), affected);
    }
    else {
      DILAY_IMPOSSIBLE
    }
  }
}

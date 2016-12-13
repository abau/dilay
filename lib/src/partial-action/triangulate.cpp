/* This file is part of Dilay
 * Copyright © 2015,2016 Alexander Bau
 * Use and redistribute under the terms of the GNU General Public License
 */
#include "adjacent-iterator.hpp"
#include "affected-faces.hpp"
#include "partial-action/insert.hpp"
#include "partial-action/triangulate.hpp"
#include "primitive/triangle.hpp"
#include "winged/edge.hpp"
#include "winged/face.hpp"
#include "winged/mesh.hpp"
#include "winged/vertex.hpp"
#include "util.hpp"

namespace {
  bool edgeHasNewVertex (const WingedMesh& mesh, const WingedEdge& edge) {
    return edge.vertex1Ref ().isNewVertex (mesh) || edge.vertex2Ref ().isNewVertex (mesh);
  }
}

namespace PartialAction {
  void triangulateQuad (WingedMesh& mesh, WingedFace& face, AffectedFaces& affected) {
    assert (face.numEdges () == 4);

    WingedEdge* edge = nullptr;

    for (WingedEdge& e : face.adjacentEdges ()) {
      if (edgeHasNewVertex (mesh, e)) {
        edge = &e;
        break;
      }
    }
    assert (edge);

    WingedEdge& newEdge = insertEdgeFaceAtNewVertex (mesh, face, *edge, affected);

    newEdge.leftFaceRef  ().writeIndices (mesh);
    newEdge.rightFaceRef ().writeIndices (mesh);
  }

  void triangulate5Gon (WingedMesh& mesh, WingedFace& face, AffectedFaces& affected) {
    assert (face.numEdges () == 5);

    WingedEdge* nonSubdividedEdge = nullptr;

    for (WingedEdge& e : face.adjacentEdges ()) {
      if (edgeHasNewVertex (mesh, e) == false) {
        nonSubdividedEdge = &e;
        break;
      }
    }
    assert (nonSubdividedEdge);

    WingedEdge& newEdge1 = insertEdgeFaceAtNewVertex ( mesh, face
                                                     , nonSubdividedEdge->predecessorRef (face)
                                                     , affected );

    newEdge1.leftFaceRef ().writeIndices (mesh);

    WingedVertex& oldVertex1 = nonSubdividedEdge->vertex1Ref ();
    WingedVertex& oldVertex2 = nonSubdividedEdge->vertex2Ref ();

    WingedEdge& newEdge2 = oldVertex1.valence () < oldVertex2.valence ()
                         ? insertEdgeFace (mesh, face, *nonSubdividedEdge, oldVertex1, affected)
                         : insertEdgeFace (mesh, face, *nonSubdividedEdge, oldVertex2, affected);

    newEdge2.leftFaceRef  ().writeIndices (mesh);
    newEdge2.rightFaceRef ().writeIndices (mesh);
  }

  void triangulate6Gon (WingedMesh& mesh, WingedFace& f, AffectedFaces& affected) {
    assert (f.numEdges () == 6);
    /*     4
     *    /c\
     *   5---3
     *  /a\f/b\
     * 0---1---2
     */
    WingedEdge& e01  = f.edgeRef ().secondVertexRef (f).isNewVertex (mesh)
                     ? f.edgeRef ()
                     : f.edgeRef ().predecessorRef (f);

    assert (e01.secondVertexRef (f).isNewVertex (mesh));

    WingedEdge& e12  = e01.successorRef (f);
    WingedEdge& e23  = e12.successorRef (f);
    WingedEdge& e34  = e23.successorRef (f);
    WingedEdge& e45  = e34.successorRef (f);
    WingedEdge& e50  = e45.successorRef (f);

    WingedVertex& v0 = e01.firstVertexRef (f);
    WingedVertex& v1 = e12.firstVertexRef (f);
    WingedVertex& v2 = e23.firstVertexRef (f);
    WingedVertex& v3 = e34.firstVertexRef (f);
    WingedVertex& v4 = e45.firstVertexRef (f);
    WingedVertex& v5 = e50.firstVertexRef (f);

    WingedFace& a = mesh.addFace (PrimTriangle (mesh,v0,v1,v5));
    WingedFace& b = mesh.addFace (PrimTriangle (mesh,v1,v2,v3));
    WingedFace& c = mesh.addFace (PrimTriangle (mesh,v3,v4,v5));

    a.edge (&e01);
    b.edge (&e23);
    c.edge (&e45);

    WingedEdge& e13 = mesh.addEdge ();
    WingedEdge& e35 = mesh.addEdge ();
    WingedEdge& e51 = mesh.addEdge ();

    e13.setGeometry (&v1,&v3,&f,&b,&e51,&e35,&e23,&e12);
    e35.setGeometry (&v3,&v5,&f,&c,&e13,&e51,&e45,&e34);
    e51.setGeometry (&v5,&v1,&f,&a,&e35,&e13,&e01,&e50);

    e01.face        (f,&a);
    e01.predecessor (a,&e50);
    e01.successor   (a,&e51);

    e12.face        (f,&b);
    e12.predecessor (b,&e13);
    e12.successor   (b,&e23);

    e23.face        (f,&b);
    e23.predecessor (b,&e12);
    e23.successor   (b,&e13);

    e34.face        (f,&c);
    e34.predecessor (c,&e35);
    e34.successor   (c,&e45);

    e45.face        (f,&c);
    e45.predecessor (c,&e34);
    e45.successor   (c,&e35);

    e50.face        (f,&a);
    e50.predecessor (a,&e51);
    e50.successor   (a,&e01);

    f.edge (&e13);

    f.writeIndices (mesh);
    a.writeIndices (mesh);
    b.writeIndices (mesh);
    c.writeIndices (mesh);

    affected.insert (f);
    affected.insert (a);
    affected.insert (b);
    affected.insert (c);
  }

  void triangulate (WingedMesh& mesh, WingedFace& face, AffectedFaces& affected) {
    switch (face.numEdges ()) {
      case 4:
        triangulateQuad (mesh, face, affected);
        break;
      case 5:
        triangulate5Gon (mesh, face, affected);
        break;
      case 6:
        triangulate6Gon (mesh, face, affected);
        break;
      default:
        DILAY_IMPOSSIBLE
    }
  }

  void splitAndTriangulate ( WingedMesh& mesh, WingedEdge& edge
                           , const glm::vec3& newPos, AffectedFaces& affected )
  {
#ifndef NDEBUG
    const unsigned int valence1 = edge.vertex1Ref ().valence ();
    const unsigned int valence2 = edge.vertex2Ref ().valence ();

    WingedVertex& v3 = edge.vertexRef (edge.leftFaceRef  (), 2);
    WingedVertex& v4 = edge.vertexRef (edge.rightFaceRef (), 2);

    const unsigned int valence3 = v3.valence ();
    const unsigned int valence4 = v4.valence ();
#endif
    WingedEdge& newEdge = insertEdgeVertex (mesh, edge, newPos, affected);

    triangulateQuad (mesh, edge.leftFaceRef  (), affected);
    triangulateQuad (mesh, edge.rightFaceRef (), affected);

    assert (newEdge.vertex2Ref ().valence () == 4);
    assert (newEdge.vertex1Ref ().valence () == valence1);
    assert (edge.vertex2Ref ().valence () == valence2);

    assert (v3.valence () == valence3 + 1);
    assert (v4.valence () == valence4 + 1);
  }

  void splitAndTriangulate ( WingedMesh& mesh, WingedFace& f
                           , const glm::vec3& newPos, AffectedFaces& affected )
  {
    assert (f.numEdges () == 3);
    /*          2                  2
     *         / \                /|\
     *        /   \              / | \
     *       /  f  \     =>     /b 3 a\
     *      /       \          / / f \ \
     *     0---------1        0---------1
     */

    WingedEdge& e01 = f.edgeRef ();
    WingedEdge& e12 = f.edgeRef ().successorRef (f,0);
    WingedEdge& e20 = f.edgeRef ().successorRef (f,1);

    WingedVertex& v0 = e01.firstVertexRef (f);
    WingedVertex& v1 = e12.firstVertexRef (f);
    WingedVertex& v2 = e20.firstVertexRef (f);

    WingedVertex& v3 = mesh.addVertex (newPos);

    WingedEdge& e03 = mesh.addEdge ();
    WingedEdge& e13 = mesh.addEdge ();
    WingedEdge& e23 = mesh.addEdge ();

    v3.edge (&e03);

    WingedFace& a = mesh.addFace (PrimTriangle (mesh, v1, v2, v3));
    WingedFace& b = mesh.addFace (PrimTriangle (mesh, v0, v3, v2));

    f.edge (&e01);
    a.edge (&e12);
    b.edge (&e20);

    e03.setGeometry (&v0, &v3, &b, &f, &e20, &e23, &e13, &e01);
    e13.setGeometry (&v1, &v3, &f, &a, &e01, &e03, &e23, &e12);
    e23.setGeometry (&v2, &v3, &a, &b, &e12, &e13, &e03, &e20);

    e01.predecessor (f, &e03);
    e01.successor   (f, &e13);

    e12.face        (f, &a);
    e12.predecessor (a, &e13);
    e12.successor   (a, &e23);

    e20.face        (f, &b);
    e20.predecessor (b, &e23);
    e20.successor   (b, &e03);

    f.writeIndices (mesh);
    a.writeIndices (mesh);
    b.writeIndices (mesh);

    affected.insert (f);
    affected.insert (a);
    affected.insert (b);
  }
}

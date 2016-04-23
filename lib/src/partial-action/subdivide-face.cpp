/* This file is part of Dilay
 * Copyright © 2015,2016 Alexander Bau
 * Use and redistribute under the terms of the GNU General Public License
 */
#include <glm/glm.hpp>
#include "affected-faces.hpp"
#include "partial-action/subdivide-face.hpp"
#include "primitive/triangle.hpp"
#include "winged/edge.hpp"
#include "winged/face.hpp"
#include "winged/mesh.hpp"
#include "winged/vertex.hpp"

void PartialAction :: subdivideFace ( WingedMesh& mesh, WingedFace& f
                                    , AffectedFaces& affectedFaces, const glm::vec3& newPos )
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

  affectedFaces.insert (f);
  affectedFaces.insert (a);
  affectedFaces.insert (b);
}

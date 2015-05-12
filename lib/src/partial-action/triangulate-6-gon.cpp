#include "affected-faces.hpp"
#include "partial-action/triangulate-6-gon.hpp"
#include "primitive/triangle.hpp"
#include "winged/edge.hpp"
#include "winged/face.hpp"
#include "winged/mesh.hpp"
#include "winged/vertex.hpp"

void PartialAction :: triangulate6Gon ( WingedMesh& mesh, WingedFace& f
                                      , AffectedFaces& affectedFaces ) 
{
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

  affectedFaces.insert (f);
  affectedFaces.insert (a);
  affectedFaces.insert (b);
  affectedFaces.insert (c);
}

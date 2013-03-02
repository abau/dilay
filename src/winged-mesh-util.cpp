#include <iostream>
#include "util.hpp"
#include "winged-mesh-util.hpp"

void WingedMeshUtil :: printStatistics (const WingedMesh& mesh) {
  auto vertexIndex = [&mesh] (ConstLinkedVertex v) {
    if (v != mesh.nullVertex ())
      return Util :: toString (v->index ());
    else
      return std::string ("NULL");
  };
  auto edgeId = [&mesh] (ConstLinkedEdge e) {
    if (e != mesh.nullEdge ())
      return Util :: toString (e->id ());
    else
      return std::string ("NULL");
  };
  auto faceId = [&mesh] (ConstLinkedFace f) {
    if (f != mesh.nullFace ())
      return Util :: toString (f->id ());
    else
      return std::string ("NULL");
  };

  std::cout << "Number of vertices: "        << mesh.numVertices () << std::endl;
  std::cout << "Number of winged vertices: " << mesh.numWingedVertices () << std::endl;
  std::cout << "Number of edges: "           << mesh.numEdges ()    << std::endl;
  std::cout << "Number of faces: "           << mesh.numFaces ()    << std::endl;

  for (VERTEX_CONST_ITERATOR(it,mesh)) {
    std::cout << "Vertex "        << it->index  () 
              << "\n\tposition: " << it->vertex (mesh)
              << "\n\tedge: "     << edgeId     (it->edge ())
              << "\n\tnormal: "   << it->normal (mesh)
              << std::endl;
  }

  for (EDGE_CONST_ITERATOR(it,mesh)) {
    std::cout << "Edge " << it->id () 
      << "\n\tvertex 1:\t\t"        << vertexIndex (it->vertex1          ())  
      <<   "\tvertex 2:\t\t"        << vertexIndex (it->vertex2          ()) 
      << "\n\tleft face:\t\t"       << faceId      (it->leftFace         ())   
      <<   "\tright face:\t\t"      << faceId      (it->rightFace        ())
      << "\n\tleft predecessor:\t"  << edgeId      (it->leftPredecessor  ())   
      <<   "\tleft successor:\t\t"  << edgeId      (it->leftSuccessor    ())   
      << "\n\tright predecessor:\t" << edgeId      (it->rightPredecessor ())   
      <<   "\tright successor:\t"   << edgeId      (it->rightSuccessor   ())   
      << std::endl;
  }

  for (FACE_CONST_ITERATOR(it,mesh)) {
    std::cout << "Face "         << it->id () 
              << "\n\tedge:\t\t" << edgeId (it->edge())
              << "\n\tnormal: "  << it->normal (mesh)
              << std::endl;
  }
}

LinkedEdge findOrAddEdge (WingedMesh&,unsigned int,unsigned int,LinkedFace);

void WingedMeshUtil :: fromMesh (WingedMesh& w, const Mesh& m) {
  w.reset ();
  // Vertices
  for (unsigned int i = 0; i < m.numVertices (); i++) {
    w.addVertex (m.vertex (i), w.nullEdge ());
  }
  // Faces & Edges
  for (unsigned int i = 0; i < m.numIndices (); i += 3) {
    unsigned int index1 = m.index (i + 0);
    unsigned int index2 = m.index (i + 1);
    unsigned int index3 = m.index (i + 2);

    LinkedFace f  = w.addFace (WingedFace (w.nullEdge ()));
    LinkedEdge e1 = findOrAddEdge (w,index1, index2, f);
    LinkedEdge e2 = findOrAddEdge (w,index2, index3, f);
    LinkedEdge e3 = findOrAddEdge (w,index3, index1, f);

    e1->setPredecessor (*f,e3);
    e1->setSuccessor   (*f,e2);
    e2->setPredecessor (*f,e1);
    e2->setSuccessor   (*f,e3);
    e3->setPredecessor (*f,e2);
    e3->setSuccessor   (*f,e1);
  }
  // Indices
  w.rebuildIndices ();
  // Normals
  w.rebuildNormals ();
}

LinkedEdge   findEdge   (WingedMesh&,unsigned int,unsigned int);
LinkedVertex findVertex (WingedMesh&,unsigned int);

LinkedEdge findOrAddEdge  ( WingedMesh& mesh, unsigned int index1
                          , unsigned int index2, LinkedFace face) {
  LinkedEdge e = findEdge (mesh, index2, index1);
  if (e != mesh.nullEdge ()) {
    e->setRightFace (face);
  }
  else {
    LinkedVertex v1 = findVertex (mesh, index1);
    LinkedVertex v2 = findVertex (mesh, index2);
    assert (v1 != mesh.nullVertex () && v2 != mesh.nullVertex ());

    e = mesh.addEdge (WingedEdge (mesh));
    e ->setVertex1  (v1);
    e ->setVertex2  (v2);
    e ->setLeftFace (face);
    v1->setEdge     (e);
    v2->setEdge     (e);
  }
  face->setEdge (e);
  return e;
}

LinkedEdge findEdge ( WingedMesh&  mesh, unsigned int vertex1, unsigned int vertex2) {
  for (EDGE_ITERATOR (eIt,mesh)) {
    if (eIt->vertex1 ()->index () == vertex1 && eIt->vertex2 ()->index () == vertex2) {
      return eIt;
    }
  }
  return mesh.nullEdge ();
}

LinkedVertex findVertex (WingedMesh&  mesh, unsigned int vertex) {
  for (VERTEX_ITERATOR(vIt,mesh)) {
    if (vIt->index () == vertex) { 
      return vIt;
    }
  }
  return mesh.nullVertex ();
}

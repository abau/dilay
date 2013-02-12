#include <iostream>
#include "util.hpp"
#include "winged-mesh-util.hpp"

void WingedMeshUtil :: printStatistics (const WingedMesh& mesh) {
  std::cout << "Number of vertices: "        << mesh.numVertices () << std::endl;
  std::cout << "Number of winged vertices: " << mesh.numWingedVertices () << std::endl;
  std::cout << "Number of edges: "           << mesh.numEdges ()    << std::endl;
  std::cout << "Number of faces: "           << mesh.numFaces ()    << std::endl;

  for ( VertexConstIterator it = mesh.vertexIterator ()
      ; it.hasElement (); it.next ()) {
    glm::vec3 v = mesh.vertex (it.data ().index ());
    glm::vec3 n = it.data ().normal (mesh);

    std::cout << "Vertex " << it.data ().index () 
              << "\n\tposition: " << Util :: toString (v)
              << "\n\tedge: " << STRING_ID(it.data ().edge ())
              << "\n\tnormal: "   << Util :: toString (n)
              << std::endl;
  }

  for ( EdgeConstIterator it = mesh.edgeIterator ()
      ; it.hasElement (); it.next ()) {
    const WingedEdge& e = it.data ();

    std::cout << "Edge " << e.id () 
      << "\n\tvertex 1:\t\t"        << STRING_INDEX(e.vertex1 ()) 
      <<   "\tvertex 2:\t\t"        << STRING_INDEX(e.vertex2 ())  
      << "\n\tleft face:\t\t"       << STRING_ID(e.leftFace ())   
      <<   "\tright face:\t\t"      << STRING_ID(e.rightFace ())
      << "\n\tleft predecessor:\t"  << STRING_ID(e.leftPredecessor ())   
      <<   "\tleft successor:\t\t"  << STRING_ID(e.leftSuccessor ())   
      << "\n\tright predecessor:\t" << STRING_ID(e.rightPredecessor ())   
      <<   "\tright successor:\t"   << STRING_ID(e.rightSuccessor ())   
      << std::endl;
  }

  for ( FaceConstIterator it = mesh.faceIterator ()
      ; it.hasElement (); it.next ()) {
    glm::vec3 n = it.data ().normal (mesh);

    std::cout << "Face "         << it.data ().id () 
              << "\n\tedge:\t\t" << STRING_ID(it.data().edge())
              << "\n\tnormal: " << Util :: toString (n)
              << std::endl;
  }
}

MaybePtr <LinkedEdge> WingedMeshUtil :: findEdge ( WingedMesh&  mesh
                                                 , unsigned int vertex1
                                                 , unsigned int vertex2) {
  EdgeIterator eIt = mesh.edgeIterator ();
  while (eIt.hasElement ()) {
    if (eIt.data ().vertex1 ()->data ().index () == vertex1 && 
        eIt.data ().vertex2 ()->data ().index () == vertex2) {
      return MaybePtr <LinkedEdge> (eIt.linkedElement ());
    }
    eIt.next ();
  }
  return MaybePtr <LinkedEdge> :: nothing ();
}

MaybePtr <LinkedVertex> WingedMeshUtil :: findVertex ( WingedMesh&  mesh
                                                     , unsigned int vertex) {
  VertexIterator vIt = mesh.vertexIterator ();
  while (vIt.hasElement ()) {
    if (vIt.data ().index () == vertex) { 
      return MaybePtr <LinkedVertex> (vIt.linkedElement ());
    }
    vIt.next ();
  }
  return MaybePtr <LinkedVertex> :: nothing ();
}

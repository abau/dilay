#include <iostream>
#include "util.hpp"
#include "winged-mesh-util.hpp"

void WingedMeshUtil :: printStatistics (const WingedMesh& mesh) {
  std::cout << "Number of vertices: " << mesh.numVertices () << std::endl;
  std::cout << "Number of edges: "    << mesh.numEdges ()    << std::endl;
  std::cout << "Number of faces: "    << mesh.numFaces ()    << std::endl;

  VertexConstIterator vIt = mesh.vertexIterator ();
  while (vIt.hasElement ()) {
    glm::vec3 v = mesh.vertex (vIt.data ().index ());

    std::cout << "Vertex " << vIt.data ().index () 
      << "\n\tposition: " << v.x << " " << v.y << " " << v.z  
      << std::endl;
    vIt.next ();
  }

  EdgeConstIterator eIt = mesh.edgeIterator ();
  while (eIt.hasElement ()) {
    const WingedEdge& e = eIt.data ();

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
    eIt.next ();
  }

  FaceConstIterator fIt = mesh.faceIterator ();
  while (fIt.hasElement ()) {
    const WingedFace& f = fIt.data ();

    std::cout << "Face "  << f.id () 
      << "\n\tdepth:\t\t" << f.depth () 
      << std::endl;
    fIt.next ();
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

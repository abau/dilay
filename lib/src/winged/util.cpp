/* This file is part of Dilay
 * Copyright © 2015,2016 Alexander Bau
 * Use and redistribute under the terms of the GNU General Public License
 */
#include <iostream>
#include "../util.hpp"
#include "adjacent-iterator.hpp"
#include "index-octree.hpp"
#include "primitive/triangle.hpp"
#include "winged/edge.hpp"
#include "winged/face.hpp"
#include "winged/mesh.hpp"
#include "winged/util.hpp"
#include "winged/vertex.hpp"

void WingedUtil :: printStatistics (const WingedMesh& mesh, const WingedVertex& v) {
  std::cout   << "vertex "         << v.index       () 
              << "\n\tposition:\t" << v.position    (mesh)
              << "\n\tedge:\t\t"   << v.edgeIndex   ();
    std::cout << "\n\tnormal:\t\t" << v.savedNormal (mesh);
  std::cout   << std::endl;
}

void WingedUtil :: printStatistics (const WingedEdge& e) {
  std::cout << "edge " << e.index ()
    << "\n\tvertex 1:\t\t"        << e.vertex1Index ()
    <<   "\tvertex 2:\t\t"        << e.vertex2Index ()
    << "\n\tleft face:\t\t"       << e.leftFaceIndex ()
    <<   "\tright face:\t\t"      << e.rightFaceIndex ()
    << "\n\tleft predecessor:\t"  << e.leftPredecessorIndex ()
    <<   "\tleft successor:\t\t"  << e.leftSuccessorIndex ()
    << "\n\tright predecessor:\t" << e.rightPredecessorIndex ()
    <<   "\tright successor:\t"   << e.rightSuccessorIndex ()
    << std::endl;
}

void WingedUtil :: printStatistics (const WingedFace& f) {
  std::cout   << "face "           << f.index () 
              << "\n\tedge:\t\t\t" << f.edgeIndex ()
              << std::endl;
}

void WingedUtil :: printStatistics (const WingedMesh& mesh, bool printAll) {
  std::cout << "mesh:\t\t\t\t"              << mesh.index ()       << std::endl;
  std::cout << "\tnumber of vertices:\t"    << mesh.numVertices () << std::endl;
  std::cout << "\tnumber of edges:\t"       << mesh.numEdges ()    << std::endl;
  std::cout << "\tnumber of faces:\t"       << mesh.numFaces ()    << std::endl;
  std::cout << "\tnumber of indices:\t"     << mesh.numIndices ()  
            << " (" << mesh.numIndices () / 3  << ")" << std::endl;

  if (printAll) {
    mesh.forEachConstVertex ([&mesh] (const WingedVertex& v) { 
      WingedUtil::printStatistics (mesh,v); 
    });

    mesh.forEachConstEdge ([] (const WingedEdge& e) {
      WingedUtil :: printStatistics (e);
    });

    mesh.forEachConstFace ([] (const WingedFace& face) {
      WingedUtil :: printStatistics (face);
    });
  }
  mesh.octree ().printStatistics ();
}

glm::vec3 WingedUtil :: averageNormal (const WingedMesh& mesh, const VertexPtrSet& vertices) {
  assert (vertices.size () > 0);

  glm::vec3 avgNormal (0.0f);
  for (WingedVertex* v : vertices) {
    avgNormal = avgNormal + v->savedNormal (mesh);
  }
  return avgNormal / float (vertices.size ());
}

glm::vec3 WingedUtil :: center (const WingedMesh& mesh, const VertexPtrSet& vertices) {
  assert (vertices.size () > 0);

  glm::vec3 c (0.0f);
  for (WingedVertex* v : vertices) {
    c += v->position (mesh);
  }
  return c / float (vertices.size ());
}

glm::vec3 WingedUtil :: center (const WingedMesh& mesh, const WingedVertex& vertex) {
  glm::vec3    c (0.0f);
  unsigned int n (0);

  for (const WingedVertex& v : vertex.adjacentVertices ()) {
    c += v.position (mesh);
    n++;
  }
  assert (n > 0);
  return c / float (n);
}

float WingedUtil :: averageLength (const WingedMesh& mesh, const EdgePtrVec& edges) {
  assert (edges.size () > 0);

  float l = 0.0f;
  for (const WingedEdge* e : edges) {
    l += e->length (mesh);
  }
  return l / float (edges.size ());
}

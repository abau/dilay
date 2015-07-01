#include <glm/glm.hpp>
#include <iostream>
#include "../util.hpp"
#include "adjacent-iterator.hpp"
#include "octree.hpp"
#include "primitive/triangle.hpp"
#include "util.hpp"
#include "winged/edge.hpp"
#include "winged/face.hpp"
#include "winged/mesh.hpp"
#include "winged/util.hpp"
#include "winged/vertex.hpp"

void WingedUtil :: printStatistics (const WingedMesh& mesh, const WingedVertex& v) {
  std::cout   << "vertex "               << v.index       () 
              << "\n\tposition:\t"       << v.position    (mesh)
              << "\n\tedge:\t\t"         << v.edge        ()->index ();
    std::cout << "\n\tnormal:\t\t"       << v.savedNormal (mesh);
  std::cout   << std::endl;
}

void WingedUtil :: printStatistics (const WingedEdge& e) {
  auto maybeEdgeIndex = [] (WingedEdge* edge) {
    return bool (edge) ? std::to_string (edge->index ())
                       : std::string ("NULL");
  };
  auto maybeFaceIndex = [] (WingedFace* face) {
    return bool (face) ? std::to_string (face->index ())
                       : std::string ("NULL");
  };
  auto maybeVertexIndex = [] (WingedVertex* vertex) {
    return bool (vertex) ? std::to_string (vertex->index ())
                         : std::string ("NULL");
  };

  std::cout << "edge " << e.index () 
    << "\n\tvertex 1:\t\t"        << maybeVertexIndex (e.vertex1          ())
    <<   "\tvertex 2:\t\t"        << maybeVertexIndex (e.vertex2          ())
    << "\n\tleft face:\t\t"       << maybeFaceIndex   (e.leftFace         ())
    <<   "\tright face:\t\t"      << maybeFaceIndex   (e.rightFace        ())
    << "\n\tleft predecessor:\t"  << maybeEdgeIndex   (e.leftPredecessor  ())
    <<   "\tleft successor:\t\t"  << maybeEdgeIndex   (e.leftSuccessor    ())
    << "\n\tright predecessor:\t" << maybeEdgeIndex   (e.rightPredecessor ())
    <<   "\tright successor:\t"   << maybeEdgeIndex   (e.rightSuccessor   ())
    << std::endl;
}

void WingedUtil :: printStatistics (const WingedFace& f) {
  std::cout   << "face "                << f.index () 
              << "\n\tedge:\t\t\t"      << f.edgeRef ().index ()
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
  WingedUtil::printStatistics (mesh.octree ());
}

void WingedUtil :: printStatistics (const Octree& octree) {
  OctreeStatistics oStats = octree.statistics ();
  std::cout << "octree:"
            << "\n\tnum nodes:\t\t"           << oStats.numNodes
            << "\n\tnum faces:\t\t"           << oStats.numFaces
            << "\n\tnum degenerated faces:\t" << oStats.numDegeneratedFaces
            << "\n\tmax faces per node:\t"    << oStats.maxFacesPerNode
            << "\n\tmin depth:\t\t"           << oStats.minDepth
            << "\n\tmax depth:\t\t"           << oStats.maxDepth
            << "\n\tfaces per node:\t\t"      << float (oStats.numFaces) / float (oStats.numNodes) 
            << std::endl;

  for (OctreeStatistics::DepthMap::value_type& e : oStats.numNodesPerDepth) {
    std::cout << "\t" << e.second << "\tnodes at depth\t" << e.first << std::endl;
  }
  for (OctreeStatistics::DepthMap::value_type& e : oStats.numFacesPerDepth) {
    std::cout << "\t" << e.second << "\tfaces at depth\t" << e.first << std::endl;
  }
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

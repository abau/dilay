#include <glm/glm.hpp>
#include <iostream>
#include "../util.hpp"
#include "octree.hpp"
#include "winged/edge.hpp"
#include "winged/face.hpp"
#include "winged/mesh.hpp"
#include "winged/util.hpp"
#include "winged/vertex.hpp"

void WingedUtil :: printStatistics (const WingedMesh& mesh, const WingedVertex& v) {
  std::cout   << "Vertex "               << v.index       () 
              << "\n\tposition:\t"       << v.position    (mesh)
              << "\n\tedge:\t\t"         << v.edge        ()->id ();
    std::cout << "\n\tnormal:\t\t"       << v.savedNormal (mesh);
  std::cout   << std::endl;
}

void WingedUtil :: printStatistics (const WingedEdge& e) {
  auto maybeEdgeId = [] (WingedEdge* edge) {
    return bool (edge) ? std::to_string (edge->id ().primitive ())
                       : std::string ("NULL");
  };
  auto maybeFaceId = [] (WingedFace* face) {
    return bool (face) ? std::to_string (face->index ())
                       : std::string ("NULL");
  };
  auto maybeIndex = [] (WingedVertex* vertex) {
    return bool (vertex) ? std::to_string (vertex->index ())
                         : std::string ("NULL");
  };

  std::cout << "Edge " << e.id () 
    << "\n\tvertex 1:\t\t"        << maybeIndex     (e.vertex1          ())
    <<   "\tvertex 2:\t\t"        << maybeIndex     (e.vertex2          ())
    << "\n\tleft face:\t\t"       << maybeFaceId    (e.leftFace         ())
    <<   "\tright face:\t\t"      << maybeFaceId    (e.rightFace        ())
    << "\n\tleft predecessor:\t"  << maybeEdgeId    (e.leftPredecessor  ())
    <<   "\tleft successor:\t\t"  << maybeEdgeId    (e.leftSuccessor    ())
    << "\n\tright predecessor:\t" << maybeEdgeId    (e.rightPredecessor ())
    <<   "\tright successor:\t"   << maybeEdgeId    (e.rightSuccessor   ())
    << std::endl;
}

void WingedUtil :: printStatistics ( const WingedMesh& mesh, const WingedFace& f
                                   , bool printDerived) 
{
  auto maybeNodeId = [] (OctreeNode* node) {
    return bool (node) ? std::to_string (node->id ().primitive ())
                       : std::string ("NULL");
  };
  std::cout   << "Face "                << f.index () 
              << "\n\tedge:\t\t\t"      << f.edgeRef ().id ()
              << "\n\toctree node:\t\t" << maybeNodeId (f.octreeNode ())
              << "\n\tindex:\t\t\t"     << f.index ();
  if (printDerived) {
    std::cout << "\n\tnormal:\t\t\t"    << f.normal  (mesh);
  }
  std::cout   << std::endl;
}

void WingedUtil :: printStatistics (const WingedMesh& mesh, bool printDerived) {
  std::cout << "Id:\t\t\t\t"                << mesh.id ()          << std::endl;
  std::cout << "Number of vertices:\t\t"    << mesh.numVertices () << std::endl;
  std::cout << "Number of edges:\t\t"       << mesh.numEdges ()    << std::endl;
  std::cout << "Number of faces:\t\t"       << mesh.numFaces ()    << std::endl;
  std::cout << "Number of indices:\t\t"     << mesh.numIndices ()  
            << " (" << mesh.numIndices () / 3  << ")" << std::endl;

  if (mesh.vertices ().size () <= 10) {
    for (const WingedVertex& v : mesh.vertices ())
      WingedUtil :: printStatistics (mesh,v);

    for (const WingedEdge& e : mesh.edges ())
      WingedUtil :: printStatistics (e);

    mesh.octree ().forEachFace ([&mesh,printDerived] (const WingedFace& face) {
      WingedUtil :: printStatistics (mesh,face,printDerived);
    });
  }
  WingedUtil::printStatistics (mesh.octree ());
}

void WingedUtil :: printStatistics (const Octree& octree) {
  OctreeStatistics oStats = octree.statistics ();
  std::cout << "Octree:"
            << "\n\tnum nodes:\t\t"           << oStats.numNodes
            << "\n\tnum faces:\t\t"           << oStats.numFaces
            << "\n\tnum degenerated faces:\t" << oStats.numDegeneratedFaces
            << "\n\tnum free face-indices:\t" << oStats.numFreeFaceIndices
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

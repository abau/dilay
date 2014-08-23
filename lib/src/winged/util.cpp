#include <iostream>
#include <glm/glm.hpp>
#include "winged/util.hpp"
#include "winged/mesh.hpp"
#include "winged/edge.hpp"
#include "winged/face.hpp"
#include "winged/vertex.hpp"
#include "../util.hpp"
#include "octree.hpp"

void WingedUtil :: printStatistics (const WingedMesh& mesh, const WingedVertex& v) {
  std::cout   << "Vertex "               << v.index       () 
              << "\n\tposition:\t"       << v.vector      (mesh)
              << "\n\tedge:\t\t"         << v.edge        ()->id ();
    std::cout << "\n\tnormal:\t\t"       << v.savedNormal (mesh);
  std::cout   << std::endl;
}

void WingedUtil :: printStatistics (const WingedEdge& e) {
  auto maybeEdgeId = [] (WingedEdge* edge) {
    if (edge) return std::to_string (edge->id ().primitive ());
    else      return std::string ("NULL");
  };
  auto maybeFaceId = [] (WingedFace* face) {
    if (face) return std::to_string (face->id ().primitive ());
    else      return std::string ("NULL");
  };
  auto maybeIndex = [] (WingedVertex* vertex) {
    if (vertex) return std::to_string (vertex->index ());
    else        return std::string ("NULL");
  };
  auto faceGradient = [] (FaceGradient g) {
    switch (g) {
      case FaceGradient::None:  return std::string ("None");
      case FaceGradient::Left:  return std::string ("Left");
      case FaceGradient::Right: return std::string ("Right");
      default: assert (false);
    };
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
    << "\n\tis T-edge:\t\t"       << e.isTEdge      ()
    <<   "\tface gradient:\t\t"   << faceGradient   (e.faceGradient     ());
  std::cout << std::endl;
}

void WingedUtil :: printStatistics ( const WingedMesh& mesh, const WingedFace& f
                                   , bool printDerived) {
  std::cout   << "Face "                << f.id () 
              << "\n\tedge:\t\t\t"      << f.edgeRef ().id ()
              << "\n\toctree node:\t\t" << f.octreeNode ()
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
  std::cout << "Number of free face indices:\t"       << mesh.numFreeFaceIndices () << std::endl;

  if (mesh.vertices ().size () <= 10) {
    for (const WingedVertex& v : mesh.vertices ())
      WingedUtil :: printStatistics (mesh,v);

    for (const WingedEdge& e : mesh.edges ())
      WingedUtil :: printStatistics (e);

    mesh.octree ().forEachConstFace ([&mesh,printDerived] (const WingedFace& face) {
      WingedUtil :: printStatistics (mesh,face,printDerived);
    });
  }
  WingedUtil::printStatistics (mesh.octree ());
}

void WingedUtil :: printStatistics (const Octree& octree) {
  OctreeStatistics oStats = octree.statistics ();
  std::cout << "Octree:"
            << "\n\tnum nodes:\t\t"        << oStats.numNodes
            << "\n\tnum faces:\t\t"        << oStats.numFaces
            << "\n\tnum interim faces:\t"  << oStats.numInterimFaces
            << "\n\tmax faces per node:\t" << oStats.maxFacesPerNode
            << "\n\tmin depth:\t\t"        << oStats.minDepth
            << "\n\tmax depth:\t\t"        << oStats.maxDepth
            << "\n\tfaces per node:\t\t"   << float (oStats.numFaces) / float (oStats.numNodes) 
            << std::endl;

  for (OctreeStatistics::DepthMap::value_type& e : oStats.numNodesPerDepth) {
    std::cout << "\t" << e.second << "\tnodes at depth\t" << e.first << std::endl;
  }
  for (OctreeStatistics::DepthMap::value_type& e : oStats.numFacesPerDepth) {
    std::cout << "\t" << e.second << "\tfaces at depth\t" << e.first << std::endl;
  }
}

#include <iostream>
#include <glm/glm.hpp>
#include "winged/util.hpp"
#include "winged/mesh.hpp"
#include "winged/edge.hpp"
#include "winged/face.hpp"
#include "winged/vertex.hpp"
#include "../util.hpp"
#include "octree.hpp"
#include "octree-util.hpp"

void WingedUtil :: printStatistics ( const WingedMesh& mesh, const WingedVertex& v
                                   , bool printDerived) {
  std::cout   << "Vertex "               << v.index   () 
              << "\n\tposition:\t"       << v.vertex  (mesh)
              << "\n\tedge:\t\t"         << v.edge    ()->id ();
  if (printDerived)
    std::cout << "\n\tnormal:\t\t"       << v.normal  (mesh);
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
    << "\n\tprivious sibling:\t"  << maybeEdgeId    (e.previousSibling  ())   
    <<   "\tnext sibling:\t\t"    << maybeEdgeId    (e.nextSibling      ())
    << "\n\tis T-edge:\t\t"       << e.isTEdge      ()
    <<   "\tface gradient:\t\t"   << faceGradient   (e.faceGradient     ())
    << "\n\tvertex gradient:\t"   <<                 e.vertexGradient   ();
  std::cout << std::endl;
}

void WingedUtil :: printStatistics ( const WingedMesh& mesh, const WingedFace& f
                                   , bool printDerived) {
  std::cout   << "Face "                     << f.id () 
              << "\n\tedge:\t\t\t"           << f.edgeRef ().id ()
              << "\n\toctree node:\t\t"      << f.octreeNode ()
              << "\n\tfirst index number:\t" << f.firstIndexNumber ();
  if (printDerived) {
    std::cout << "\n\tnormal:\t\t\t"         << f.normal  (mesh);
  }
  std::cout   << std::endl;
}

void WingedUtil :: printStatistics (const WingedMesh& mesh, bool printDerived) {
  std::cout << "Id:\t\t\t\t"              << mesh.id () << std::endl;
  std::cout << "Number of vertices:\t\t"    << mesh.numVertices () << std::endl;
  std::cout << "Number of edges:\t\t"       << mesh.numEdges ()    << std::endl;
  std::cout << "Number of faces:\t\t"       << mesh.numFaces ()    << std::endl;
  std::cout << "Number of indices:\t\t"     << mesh.numIndices ()  << std::endl;

  if (mesh.vertices ().size () <= 10) {
    for (const WingedVertex& v : mesh.vertices ())
      WingedUtil :: printStatistics (mesh,v,printDerived);

    for (const WingedEdge& e : mesh.edges ())
      WingedUtil :: printStatistics (e);


    for (auto it = mesh.octree ().faceIterator (); it.isValid (); it.next ())
      WingedUtil :: printStatistics (mesh,it.element (),printDerived);
  }
  OctreeUtil :: printStatistics (mesh.octree ());
}

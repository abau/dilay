#include <iostream>
#include <limits>
#include <glm/glm.hpp>
#include <vector>
#include <map>
#include <utility>
#include <functional>
#include "winged-util.hpp"
#include "winged-vertex.hpp"
#include "winged-edge.hpp"
#include "winged-face.hpp"
#include "winged-mesh.hpp"
#include "adjacent-iterator.hpp"
#include "mesh.hpp"
#include "util.hpp"
#include "triangle.hpp"
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
  std::cout << "Number of vertices:\t\t"      << mesh.numVertices () << std::endl;
  std::cout << "Number of winged vertices:\t" << mesh.numWingedVertices () << std::endl;
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

void WingedUtil :: fromMesh (WingedMesh& w, const Mesh& m) {
  typedef std::pair  <unsigned int,unsigned int> uiPair;
  std::vector        <WingedVertex*>             vecVertices (m.numVertices ());
  std::map           <uiPair, WingedEdge*>       mapEdges;

  /** `findOrAddEdge (i1,i2,f)` searches an edge with indices `(i2,i1)` (in that order).
   * If such an edge exists, `f` becomes its new right face.
   * Otherwise a new edge is added with `f` being its left face.
   * The found (resp. created) edge is returned
   */
  auto findOrAddEdge = [&w,&vecVertices,&mapEdges] 
    (unsigned int index1, unsigned int index2, WingedFace& face) -> WingedEdge* {
      auto result = mapEdges.find (uiPair (index2, index1));
      if (result == mapEdges.end ()) {
        WingedVertex* v1    = vecVertices [index1];
        WingedVertex* v2    = vecVertices [index2];
        WingedEdge& newEdge = w.addEdge (WingedEdge ());

        mapEdges.insert (std::pair <uiPair,WingedEdge*> ( uiPair (index1,index2)
                                                        , &newEdge ));
        newEdge.vertex1  (v1);
        newEdge.vertex2  (v2);
        newEdge.leftFace (&face);
        v1->edge         (&newEdge);
        v2->edge         (&newEdge);
        face.edge        (&newEdge);

        return &newEdge;
      }
      else {
        WingedEdge* existingEdge = result->second;
        existingEdge->rightFace (&face);
        face.edge               (existingEdge);
        return existingEdge;
      }
    };

  glm::vec3 maxVertex (std::numeric_limits <float>::min ());
  glm::vec3 minVertex (std::numeric_limits <float>::max ());

  // Octree
  for (unsigned int i = 0; i < m.numVertices (); i++) {
    glm::vec3 v = m.vertex (i);
    maxVertex = glm::max (maxVertex, v);
    minVertex = glm::min (minVertex, v);
  }
  glm::vec3 center = (maxVertex + minVertex) * glm::vec3 (0.5f);
  glm::vec3 delta  =  maxVertex - minVertex;
  float     width  = glm::max (glm::max (delta.x, delta.y), delta.z);

  w.reset (center,width);

  // Vertices
  for (unsigned int i = 0; i < m.numVertices (); i++) {
    vecVertices [i] = &w.addVertex (m.vertex (i));
  }

  // Faces & Edges
  for (unsigned int i = 0; i < m.numIndices (); i += 3) {
    unsigned int index1 = m.index (i + 0);
    unsigned int index2 = m.index (i + 1);
    unsigned int index3 = m.index (i + 2);

    WingedFace& f = w.addFace (WingedFace (), Triangle ( w
                                                       , *vecVertices [index1]
                                                       , *vecVertices [index2]
                                                       , *vecVertices [index3]
                                                       ));
    WingedEdge* e1 = findOrAddEdge (index1, index2, f);
    WingedEdge* e2 = findOrAddEdge (index2, index3, f);
    WingedEdge* e3 = findOrAddEdge (index3, index1, f);

    e1->predecessor (f,e3);
    e1->successor   (f,e2);
    e2->predecessor (f,e1);
    e2->successor   (f,e3);
    e3->predecessor (f,e2);
    e3->successor   (f,e1);
  }
  w.write      ();
  w.bufferData ();
}

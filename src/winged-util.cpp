#include <iostream>
#include <limits>
#include <glm/glm.hpp>
#include <vector>
#include <map>
#include <utility>
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

namespace std {
  size_t hash <LinkedEdge> :: operator() (const LinkedEdge& l) const { 
    return hash <WingedEdge*>() (&*l);
  }
  size_t hash <LinkedFace> :: operator() (const LinkedFace& l) const { 
    return hash <WingedFace*>() (&*l);
  }
}

void WingedUtil :: printStatistics ( const WingedMesh& mesh, const WingedVertex& v
                                   , bool printDerived) {
  std::cout   << "Vertex "               << v.index   () 
              << "\n\tposition:\t"       << v.vertex  (mesh)
              << "\n\tedge:\t\t"         << v.edge    ()->id ()
              << "\n\tlevel:\t\t"        << v.level   ();
  if (printDerived)
    std::cout << "\n\tnormal:\t\t"       << v.normal  (mesh);
  std::cout   << std::endl;
}

void WingedUtil :: printStatistics (const WingedEdge& e) {
  auto siblingId = [] (const Maybe <LinkedEdge>& m) {
    if (m.isDefined ())
      return std::to_string (m.data ()->id ());
    else
      return std::string ("NULL");
  };

  std::cout << "Edge " << e.id () 
    << "\n\tvertex 1:\t\t"        << e.vertex1          ()->index ()  
    <<   "\tvertex 2:\t\t"        << e.vertex2          ()->index () 
    << "\n\tleft face:\t\t"       << e.leftFace         ()->id ()   
    <<   "\tright face:\t\t"      << e.rightFace        ()->id ()
    << "\n\tleft predecessor:\t"  << e.leftPredecessor  ()->id ()   
    <<   "\tleft successor:\t\t"  << e.leftSuccessor    ()->id ()   
    << "\n\tright predecessor:\t" << e.rightPredecessor ()->id ()   
    <<   "\tright successor:\t"   << e.rightSuccessor   ()->id ()   
    << "\n\tprivious sibling:\t"  << siblingId          (e.previousSibling ())   
    <<   "\tnext sibling:\t\t"    << siblingId          (e.nextSibling     ())
    << "\n\tis T-edge:\t\t"       << e.isTEdge ();
  std::cout << std::endl;
}

void WingedUtil :: printStatistics ( const WingedMesh& mesh, const WingedFace& f
                                   , bool printDerived) {
  std::cout   << "Face "          << f.id () 
              << "\n\tedge:\t"    << f.edge()->id ()
              << "\n\tlevel:\t"   << f.level ();
  if (printDerived)
    std::cout << "\n\tnormal: "   << f.normal (mesh);
  std::cout   << std::endl;
}

void WingedUtil :: printStatistics (const WingedMesh& mesh, bool printDerived) {
  std::cout << "Number of vertices: "        << mesh.numVertices () << std::endl;
  std::cout << "Number of winged vertices: " << mesh.numWingedVertices () << std::endl;
  std::cout << "Number of edges: "           << mesh.numEdges ()    << std::endl;
  std::cout << "Number of faces: "           << mesh.numFaces ()    << std::endl;

  if (mesh.vertices ().size () <= 10) {
    for (const WingedVertex& v : mesh.vertices ())
      WingedUtil :: printStatistics (mesh,v,printDerived);

    for (const WingedEdge& e : mesh.edges ())
      WingedUtil :: printStatistics (e);


    for (auto it = mesh.octree ().faceIterator (); it.hasFace (); it.next ())
      WingedUtil :: printStatistics (mesh,*it.face (),printDerived);
  }
  OctreeUtil :: printStatistics (mesh.octree ());
}

void WingedUtil :: fromMesh (WingedMesh& w, const Mesh& m) {
  typedef std::pair  <unsigned int,unsigned int> uiPair;
  std::vector        <LinkedVertex>              vecVertices (m.numVertices ());
  std::map           <uiPair, LinkedEdge>        mapEdges;

  /** `findOrAddEdge (i1,i2,f)` searches an edge with indices `(i2,i1)` (in that order).
   * If such an edge exists, `f` becomes its new right face.
   * Otherwise a new edge is added with `f` being its left face.
   * The found (resp. created) edge is returned
   */
  std::function <LinkedEdge (unsigned int,unsigned int,LinkedFace)> findOrAddEdge =
    [&w,&vecVertices,&mapEdges] 
    (unsigned int index1, unsigned int index2, LinkedFace face) {

      auto result = mapEdges.find (uiPair (index2, index1));
      if (result == mapEdges.end ()) {
        LinkedVertex v1    = vecVertices [index1];
        LinkedVertex v2    = vecVertices [index2];
        LinkedEdge newEdge = w.addEdge (WingedEdge ());

        mapEdges.insert (std::pair <uiPair,LinkedEdge> ( uiPair (index1,index2)
                                                       , newEdge ));
        newEdge->setVertex1  (v1);
        newEdge->setVertex2  (v2);
        newEdge->setLeftFace (face);
        v1->setEdge          (newEdge);
        v2->setEdge          (newEdge);
        face->setEdge        (newEdge);

        return newEdge;
      }
      else {
        LinkedEdge existingEdge = result->second;
        existingEdge->setRightFace (face);
        face->setEdge              (existingEdge);

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
    vecVertices [i] = w.addVertex (m.vertex (i));
  }

  // Faces & Edges
  for (unsigned int i = 0; i < m.numIndices (); i += 3) {
    unsigned int index1 = m.index (i + 0);
    unsigned int index2 = m.index (i + 1);
    unsigned int index3 = m.index (i + 2);

    LinkedFace f  = w.addFace (WingedFace (), Triangle ( w
                                                       , vecVertices [index1]
                                                       , vecVertices [index2]
                                                       , vecVertices [index3]
                                                       ));
    LinkedEdge e1 = findOrAddEdge (index1, index2, f);
    LinkedEdge e2 = findOrAddEdge (index2, index3, f);
    LinkedEdge e3 = findOrAddEdge (index3, index1, f);

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

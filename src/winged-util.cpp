#include <iostream>
#include <limits>
#include <glm/glm.hpp>
#include "winged-util.hpp"
#include "winged-vertex.hpp"
#include "winged-edge.hpp"
#include "winged-face.hpp"
#include "winged-mesh.hpp"
#include "adjacent-iterator.hpp"
#include "mesh.hpp"
#include "util.hpp"

namespace std {
  size_t hash <LinkedEdge> :: operator() (const LinkedEdge& l) const { 
    return hash <WingedEdge*>() (&*l);
  }
  size_t hash <LinkedFace> :: operator() (const LinkedFace& l) const { 
    return hash <WingedFace*>() (&*l);
  }
}

void WingedUtil :: printStatistics (const WingedMesh& mesh, bool printDerived) {
  auto siblingId = [] (const Maybe <LinkedEdge>& m) {
    if (m.isDefined ())
      return Util :: toString (m.data ()->id ());
    else
      return std::string ("no sibling");
  };

  std::cout << "Number of vertices: "        << mesh.numVertices () << std::endl;
  std::cout << "Number of winged vertices: " << mesh.numWingedVertices () << std::endl;
  std::cout << "Number of edges: "           << mesh.numEdges ()    << std::endl;
  std::cout << "Number of faces: "           << mesh.numFaces ()    << std::endl;

  for (const WingedVertex& v : mesh.vertices ()) {
    std::cout   << "Vertex "               << v.index   () 
                << "\n\tposition:\t"       << v.vertex  (mesh)
                << "\n\tedge:\t\t"         << v.edge    ()->id ()
                << "\n\tlevel:\t\t"        << v.level   ();
    if (printDerived)
      std::cout << "\n\tnormal:\t\t"       << v.normal  (mesh);
    std::cout   << std::endl;
  }

  for (const WingedEdge& e : mesh.edges ()) {
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

  for (const WingedFace& f : mesh.faces ()) {
    std::cout   << "Face "          << f.id () 
                << "\n\tedge:\t"    << f.edge()->id ()
                << "\n\tlevel:\t"   << f.level ();
    if (printDerived)
      std::cout << "\n\tnormal: "   << f.normal (mesh);
    std::cout   << std::endl;
  }
}

LinkedEdge findOrAddEdge (WingedMesh&,unsigned int,unsigned int,LinkedFace);

void WingedUtil :: fromMesh (WingedMesh& w, const Mesh& m) {
  w.reset ();
  // Vertices
  for (unsigned int i = 0; i < m.numVertices (); i++) {
    w.addVertex (m.vertex (i));
  }
  // Faces & Edges
  for (unsigned int i = 0; i < m.numIndices (); i += 3) {
    unsigned int index1 = m.index (i + 0);
    unsigned int index2 = m.index (i + 1);
    unsigned int index3 = m.index (i + 2);

    LinkedFace f  = w.addFace (WingedFace ());
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

EdgeSet WingedUtil :: edgesFromFaces (const FaceSet& faces) {
  EdgeSet edges;
  for (const LinkedFace& face : faces) {
    for (ADJACENT_EDGE_ITERATOR (it, *face)) {
      edges.insert (it.edge ());
    }
  }
  return edges;
}

// Internal /////////

LinkedEdge findOrAddEdge  ( WingedMesh& mesh, unsigned int index1
                          , unsigned int index2, LinkedFace face) {
  Maybe <LinkedEdge> e = mesh.edgeByVertexIndices (index2, index1);
  if (e.isDefined ()) {
    e.data ()->setRightFace (face);
  }
  else {
    Maybe <LinkedVertex> v1 = mesh.vertexByIndex (index1);
    Maybe <LinkedVertex> v2 = mesh.vertexByIndex (index2);
    assert (v1.isDefined () && v2.isDefined ());

    e.data  (mesh.addEdge (WingedEdge ()));
    e.data  ()->setVertex1  (v1.data ());
    e.data  ()->setVertex2  (v2.data ());
    e.data  ()->setLeftFace (face);
    v1.data ()->setEdge     (e.data ());
    v2.data ()->setEdge     (e.data ());
  }
  face->setEdge (e.data ());
  return e.data ();
}

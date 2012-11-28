#include <iostream>
#include <limits>
#include "winged-mesh.hpp"
#include "util.hpp"
#include "depth.hpp"

void WingedMesh :: addIndex (unsigned int index) {
  this->mesh.addIndex (index);
}

LinkedVertex* WingedMesh :: addVertex ( const glm::vec3& v, LinkedEdge* e) {
  unsigned int index = this->mesh.addVertex (v);
  return this->vertices.insertBack (WingedVertex (index, e));
}

LinkedEdge* WingedMesh :: addEdge (const WingedEdge& e) {
  return this->edges.insertBack (e);
}

LinkedFace* WingedMesh :: addFace (const WingedFace& f) {
  return this->faces.insertBack (f);
}

VertexIterator WingedMesh :: vertexIterator () { 
  return this->vertices.frontIterator ();
}
EdgeIterator WingedMesh :: edgeIterator () {
  return this->edges.frontIterator ();
}
FaceIterator WingedMesh :: faceIterator () {
  return this->faces.frontIterator ();
}

VertexConstIterator WingedMesh :: vertexIterator () const { 
  return this->vertices.frontIterator ();
}
EdgeConstIterator WingedMesh :: edgeIterator () const {
  return this->edges.frontIterator ();
}
FaceConstIterator WingedMesh :: faceIterator () const {
  return this->faces.frontIterator ();
}

unsigned int WingedMesh :: numVertices () const { 
  return this->vertices.numElements (); 
}

unsigned int WingedMesh :: numEdges () const { 
  return this->edges.numElements (); 
}

unsigned int WingedMesh :: numFaces () const { 
  return this->faces.numElements (); 
}

glm::vec3 WingedMesh :: vertex  (unsigned int index) const { 
  return mesh.vertex (index);
}

void WingedMesh :: rebuildIndices () {
  this->mesh.clearIndices ();
  this->addIndices ();
}

void WingedMesh :: bufferData () { this->mesh.bufferData (); }
void WingedMesh :: render     () { this->mesh.render (); }

void WingedMesh :: printStats () const {
  std::cout << "Number of vertices: " << this->numVertices () << std::endl;
  std::cout << "Number of edges: "    << this->numEdges ()    << std::endl;
  std::cout << "Number of faces: "    << this->numFaces ()    << std::endl;

  VertexConstIterator vIt = this->vertexIterator ();
  while (vIt.hasElement ()) {
    glm::vec3 v = this->vertex (vIt.data ().index ());

    std::cout << "Vertex " << vIt.data ().index () 
      << "\n\tposition: " << v.x << " " << v.y << " " << v.z  
      << std::endl;
    vIt.next ();
  }

  EdgeConstIterator eIt = this->edgeIterator ();
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

  FaceConstIterator fIt = this->faceIterator ();
  while (fIt.hasElement ()) {
    const WingedFace& f = fIt.data ();

    std::cout << "Face "  << f.id () 
      << "\n\tdepth:\t\t" << f.depth () 
      << std::endl;
    fIt.next ();
  }
}

Maybe <FaceIntersection> WingedMesh :: intersectRay (const Ray& ray) {
  FaceIterator it                       = this->faceIterator ();
  Maybe <FaceIntersection> intersection = Maybe <FaceIntersection> :: nothing ();
  float                    minDistance  = std :: numeric_limits <float> :: max ();


  while (it.hasElement ()) {
    Maybe <glm::vec3> i = it.data ().triangle (*this).intersectRay (ray); 

    if (i.isDefined ()) {
      float d = glm::distance (ray.origin (), i.data ());

      if (intersection.isNothing () || d < minDistance) {
        intersection = FaceIntersection (i.data (), *it.linkedElement ());
        minDistance  = d;
      }
    }
    it.next ();
  }
  return intersection;
}

WingedMesh* WingedMesh :: triangle ( const glm::vec3& a, const glm::vec3& b
                                  , const glm::vec3& c) {
  WingedMesh* m = new WingedMesh;

  LinkedVertex* v1 = m->addVertex (a, 0);
  LinkedVertex* v2 = m->addVertex (b, 0);
  LinkedVertex* v3 = m->addVertex (c, 0);

  LinkedFace*   f  = m->addFace   (WingedFace (0,Depth :: null ()));

  LinkedEdge*   e1 = m->addEdge   (WingedEdge (v1, v2, f, 0, 0, 0, 0, 0 ));
  LinkedEdge*   e2 = m->addEdge   (WingedEdge (v2, v3, f, 0, 0, 0, 0, 0 ));
  LinkedEdge*   e3 = m->addEdge   (WingedEdge (v3, v1, f, 0, 0, 0, 0, 0 ));

  v1->data ().setEdge (e1);
  v2->data ().setEdge (e2);
  v3->data ().setEdge (e3);

  f ->data ().setEdge (e1);

  e1->data ().setLeftPredecessor (e3);
  e1->data ().setLeftSuccessor   (e2);

  e2->data ().setLeftPredecessor (e1);
  e2->data ().setLeftSuccessor   (e3);

  e3->data ().setLeftPredecessor (e2);
  e3->data ().setLeftSuccessor   (e1);

  m->addIndices ();

  return m;
}

void WingedMesh :: addIndices () {
  FaceIterator it = this->faceIterator ();
  while (it.hasElement ()) {
    it.data ().addIndices (*this);
    it.next ();
  }
}

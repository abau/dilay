#include <limits>
#include "winged-mesh.hpp"
#include "winged-mesh-util.hpp"

void WingedMesh :: addIndex (unsigned int index) {
  this->mesh.addIndex (index);
}

LinkedVertex WingedMesh :: addVertex (const glm::vec3& v, LinkedEdge e) {
  unsigned int index = this->mesh.addVertex (v);
  this->vertices.push_back (WingedVertex (index, e));
  return --this->vertices.end ();
}

LinkedEdge WingedMesh :: addEdge (const WingedEdge& e) {
  this->edges.push_back (e);
  return --this->edges.end ();
}

LinkedFace WingedMesh :: addFace (const WingedFace& f) {
  this->faces.push_back (f);
  return --this->faces.end ();
}

VertexIterator WingedMesh :: vertexIterator () { 
  return this->vertices.begin ();
}
EdgeIterator WingedMesh :: edgeIterator () {
  return this->edges.begin ();
}
FaceIterator WingedMesh :: faceIterator () {
  return this->faces.begin ();
}

VertexConstIterator WingedMesh :: vertexIterator () const { 
  return this->vertices.cbegin ();
}
EdgeConstIterator WingedMesh :: edgeIterator () const {
  return this->edges.cbegin ();
}
FaceConstIterator WingedMesh :: faceIterator () const {
  return this->faces.cbegin ();
}

unsigned int WingedMesh :: numVertices () const { 
  return this->mesh.numVertices (); 
}

unsigned int WingedMesh :: numWingedVertices () const { 
  return this->vertices.size (); 
}

unsigned int WingedMesh :: numEdges () const { 
  return this->edges.size (); 
}

unsigned int WingedMesh :: numFaces () const { 
  return this->faces.size (); 
}

glm::vec3 WingedMesh :: vertex  (unsigned int index) const { 
  return mesh.vertex (index);
}

void WingedMesh :: rebuildIndices () {
  this->mesh.clearIndices ();
  for (WingedFace& f : this->faces) {
    f.addIndices (*this);
  }
}

void WingedMesh :: rebuildNormals () {
  this->mesh.clearNormals ();
  for (WingedVertex& v : this->vertices) {
    this->mesh.addNormal (v.normal (*this));
  }
}

void WingedMesh :: reset () {
  this->mesh    .reset ();
  this->vertices.clear ();
  this->edges   .clear ();
  this->faces   .clear ();
}

Maybe <FaceIntersection> WingedMesh :: intersectRay (const Ray& ray) {
  Maybe <FaceIntersection> intersection = Maybe <FaceIntersection> :: nothing ();
  float                    minDistance  = std :: numeric_limits <float> :: max ();

  for (FACE_ITERATOR(fIt,*this)) {
    Maybe <glm::vec3> i = fIt->triangle (*this).intersectRay (ray); 

    if (i.isDefined ()) {
      float d = glm::distance (ray.origin (), i.data ());

      if (intersection.isNothing () || d < minDistance) {
        intersection = FaceIntersection (i.data (), fIt);
        minDistance  = d;
      }
    }
  }
  return intersection;
}

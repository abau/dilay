#include <limits>
#include "winged-mesh.hpp"
#include "winged-mesh-util.hpp"
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

void WingedMesh :: reset () {
  this->mesh    .reset ();
  this->vertices.eraseAll ();
  this->edges   .eraseAll ();
  this->faces   .eraseAll ();
}

void WingedMesh :: fromMesh (const Mesh& m) {
  this->reset ();
  this->mesh = m;

  for (unsigned int i = 0; i < m.numIndices (); i += 3) {
    unsigned int index1 = m.index (i + 0);
    unsigned int index2 = m.index (i + 1);
    unsigned int index3 = m.index (i + 2);

    LinkedFace*  f      = this->addFace (WingedFace (0,Depth :: null ()));
    LinkedEdge*  e1     = this->findOrAddEdge (index1, index2, f);
    LinkedEdge*  e2     = this->findOrAddEdge (index2, index3, f);
    LinkedEdge*  e3     = this->findOrAddEdge (index3, index1, f);

    e1->data ().setPredecessor (f->data (),e3);
    e1->data ().setSuccessor   (f->data (),e2);
    e2->data ().setPredecessor (f->data (),e1);
    e2->data ().setSuccessor   (f->data (),e3);
    e3->data ().setPredecessor (f->data (),e2);
    e3->data ().setSuccessor   (f->data (),e1);
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

void WingedMesh :: addIndices () {
  FaceIterator it = this->faceIterator ();
  while (it.hasElement ()) {
    it.data ().addIndices (*this);
    it.next ();
  }
}

LinkedEdge* WingedMesh :: findOrAddEdge  ( unsigned int index1, unsigned int index2
                                         , LinkedFace* face) {

  LinkedEdge*  e     = 0;
  MaybePtr <LinkedEdge> findEdge = WingedMeshUtil :: findEdge (*this, index2, index1);
  if (findEdge.isDefined ()) {
    e = findEdge.ptr ();
    e->data ().setRightFace (face);
  }
  else {
    LinkedVertex* v1 = this->vertices.insertBack (WingedVertex (index1, 0));
    LinkedVertex* v2 = this->vertices.insertBack (WingedVertex (index2, 0));
                  e  = this->addEdge (WingedEdge (v1, v2, face, 0, 0, 0, 0, 0 ));
    v1->data ().setEdge     (e);
    v2->data ().setEdge     (e);
    e ->data ().setLeftFace (face);
  }
  if (face != 0)
    face->data ().setEdge (e);
  return e;
}

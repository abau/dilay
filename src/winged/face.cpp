#include <limits>
#include <algorithm>
#include <glm/glm.hpp>
#include "winged/vertex.hpp"
#include "winged/edge.hpp"
#include "winged/face.hpp"
#include "winged/mesh.hpp"
#include "adjacent-iterator.hpp"
#include "triangle.hpp"
#include "octree.hpp"

WingedFace :: WingedFace () : WingedFace (nullptr, Id (), nullptr, 0) {}

WingedFace :: WingedFace (WingedEdge* e, const Id& id, OctreeNode* n, unsigned int fin)
  : _id               (id)
  , _edge             (e)
  , _octreeNode       (n) 
  , _firstIndexNumber (fin)
  {}

void WingedFace :: writeIndices (WingedMesh& mesh, const unsigned int *newFIN) {
  assert (this->isTriangle ());
  if (newFIN) {
    this->_firstIndexNumber = *newFIN;
  }
  unsigned int indexNumber = this->_firstIndexNumber;
  for (ADJACENT_VERTEX_ITERATOR (it,*this)) {
    it.element ().writeIndex (mesh,indexNumber);
    indexNumber++;
  }
}

void WingedFace :: writeNormals (WingedMesh& mesh) {
  for (ADJACENT_VERTEX_ITERATOR (it,*this)) {
    it.element ().writeNormal (mesh);
  }
}

void WingedFace :: write (WingedMesh& mesh, const unsigned int *newFIN) {
  this->writeIndices (mesh, newFIN);
  this->writeNormals (mesh);
}

Triangle WingedFace :: triangle (const WingedMesh& mesh) const {
  assert (this->isTriangle ());

  return Triangle ( this->firstVertex  ().vertex (mesh)
                  , this->secondVertex ().vertex (mesh)
                  , this->thirdVertex  ().vertex (mesh)
                  );
}

WingedVertex& WingedFace :: firstVertex () const { 
  return this->_edge->firstVertexRef (*this);
}

WingedVertex& WingedFace :: secondVertex () const { 
  return this->_edge->secondVertexRef (*this);
}

WingedVertex& WingedFace :: thirdVertex () const { 
  return this->_edge->successor (*this)->secondVertexRef (*this);
}

unsigned int WingedFace :: numEdges () const {
  unsigned int i = 0;

  for (ADJACENT_EDGE_ITERATOR(_,*this)) {
    i++;
  }
  return i;
}

glm::vec3 WingedFace :: normal (const WingedMesh& mesh) const {
  glm::vec3 v1 = this->_edge->vertex (*this,0)->vertex (mesh);
  glm::vec3 v2 = this->_edge->vertex (*this,1)->vertex (mesh);
  glm::vec3 v3 = this->_edge->vertex (*this,2)->vertex (mesh);

  return glm::normalize (glm::cross (v2-v1, v3-v2));
}

WingedEdge* WingedFace :: adjacent (const WingedVertex& vertex, bool skipTEdges) const {
  for (auto it = this->adjacentEdgeIterator (skipTEdges); it.isValid (); it.next ()) {
    if (it.element ().isAdjacent (vertex))
      return &it.element ();
  }
  assert (false);
}

WingedEdge* WingedFace :: longestEdge (const WingedMesh& mesh) const {
  WingedEdge* longest   = this->edge ();
  float       maxLength = 0.0f;

  for (ADJACENT_EDGE_ITERATOR (it,*this)) {
    float length = it.element ().length (mesh);
    if (length > maxLength) {
      maxLength = length;
      longest   = &it.element ();
    }
  }
  return longest;
}

WingedVertex* WingedFace :: tVertex () const {
  for (ADJACENT_VERTEX_ITERATOR (it,*this)) {
    if (it.element ().tEdge ())
      return &it.element ();
  }
  return nullptr;
}

WingedEdge* WingedFace :: tEdge () const {
  for (ADJACENT_EDGE_ITERATOR (it,*this)) {
    if (it.element ().isTEdge ())
      return &it.element ();
  }
  return nullptr;
}

bool WingedFace :: isTriangle () const { return this->numEdges () == 3; }

float WingedFace :: incircleRadius  (const WingedMesh& mesh) const {
  assert (this->isTriangle ());
  glm::vec3 v1 = this->firstVertex  ().vertex (mesh);
  glm::vec3 v2 = this->secondVertex ().vertex (mesh);
  glm::vec3 v3 = this->thirdVertex  ().vertex (mesh);

  const float a = glm::length (v1 - v2);
  const float b = glm::length (v2 - v3);
  const float c = glm::length (v3 - v1);
  const float s = (a + b + c) * 0.5f;

  return glm::sqrt ((s-a) * (s-b) * (s-c) / s);
}

WingedVertex* WingedFace :: designatedTVertex () const {
  for (ADJACENT_VERTEX_ITERATOR (it,*this)) {
    WingedVertex& v  = it.element ();
    WingedEdge&   e1 = *it.edge ();
    WingedEdge&   e2 = e1.adjacentRef (*this,v);
    WingedFace&   o1 = e1.otherFaceRef (*this);
    WingedFace&   o2 = e2.otherFaceRef (*this);

    if (e1.gradientAlong (o1) && e2.gradientAlong (o2)) {
      return &v;
    }
  }
  assert (false);
}

AdjacentEdgeIterator WingedFace :: adjacentEdgeIterator (bool skipT) const {
  return AdjacentEdgeIterator (*this,skipT);
}
AdjacentVertexIterator WingedFace :: adjacentVertexIterator (bool skipT) const {
  return AdjacentVertexIterator (*this,skipT);
}
AdjacentFaceIterator WingedFace :: adjacentFaceIterator (bool skipT) const {
  return AdjacentFaceIterator (*this,skipT);
}
AdjacentEdgeIterator WingedFace :: adjacentEdgeIterator ( WingedEdge& e
                                                        , bool skipT) const {
  return AdjacentEdgeIterator (*this,e,skipT);
}
AdjacentVertexIterator WingedFace :: adjacentVertexIterator ( WingedEdge& e
                                                            , bool skipT) const {
  return AdjacentVertexIterator (*this,e,skipT);
}
AdjacentFaceIterator WingedFace :: adjacentFaceIterator ( WingedEdge& e
                                                        , bool skipT) const {
  return AdjacentFaceIterator (*this,e,skipT);
}

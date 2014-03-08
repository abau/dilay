#include <glm/glm.hpp>
#include "winged/mesh.hpp"
#include "winged/face.hpp"
#include "winged/edge.hpp"
#include "winged/vertex.hpp"
#include "adjacent-iterator.hpp"
#include "sphere.hpp"

WingedVertex :: WingedVertex (unsigned int i, WingedEdge* e) 
  : _index (i), _edge (e) {}

void WingedVertex :: edge (WingedEdge* e) { this->_edge = e; }

unsigned int WingedVertex :: writeIndex (WingedMesh& mesh) {
  return mesh.addIndex (this->_index);
}

void WingedVertex :: writeIndex (WingedMesh& mesh, unsigned int indexNumber) {
  mesh.setIndex (indexNumber, this->_index);
}

glm::vec3 WingedVertex :: vertex (const WingedMesh& mesh) const {
  return mesh.vertex (this->_index);
}

glm::vec3 WingedVertex :: normal (const WingedMesh& mesh) const {
  glm::vec3 normal = glm::vec3 (0.0f,0.0f,0.0f);

  for (ADJACENT_FACE_ITERATOR (it,*this)) {
    normal = normal + it.element ().normal (mesh);
  }
  return glm::normalize (normal);
}

void WingedVertex :: writeNormal (WingedMesh& mesh) const {
  mesh.setNormal (this->_index, this->normal (mesh));
}

unsigned int WingedVertex :: valence () const {
  unsigned int i = 0;
  for (ADJACENT_EDGE_ITERATOR (it,*this))
    ++i;
  return i;
}

WingedEdge* WingedVertex :: tEdge () const {
  for (ADJACENT_EDGE_ITERATOR (it,*this)) {
    if (it.element ().isTEdge ())
      return &it.element ();
  }
  return nullptr;
}

bool WingedVertex :: intersects (const WingedMesh& mesh, const Sphere& sphere) const {
  return sphere.intersects (this->vertex (mesh));
}

AdjacentEdgeIterator WingedVertex :: adjacentEdgeIterator (bool skipT) const {
  return AdjacentEdgeIterator (*this,skipT);
}
AdjacentVertexIterator WingedVertex :: adjacentVertexIterator (bool skipT) const {
  return AdjacentVertexIterator (*this,skipT);
}
AdjacentFaceIterator WingedVertex :: adjacentFaceIterator () const {
  return AdjacentFaceIterator (*this);
}
AdjacentEdgeIterator WingedVertex :: adjacentEdgeIterator ( WingedEdge& e
                                                          , bool skipT) const {
  return AdjacentEdgeIterator (*this,e,skipT);
}
AdjacentVertexIterator WingedVertex :: adjacentVertexIterator ( WingedEdge& e
                                                              , bool skipT ) const {
  return AdjacentVertexIterator (*this,e,skipT);
}
AdjacentFaceIterator WingedVertex :: adjacentFaceIterator (WingedEdge& e) const {
  return AdjacentFaceIterator (*this,e);
}

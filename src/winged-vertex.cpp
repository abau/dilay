#include <glm/glm.hpp>
#include "winged-mesh.hpp"
#include "winged-face.hpp"
#include "winged-edge.hpp"
#include "winged-vertex.hpp"
#include "adjacent-iterator.hpp"
#include "maybe.hpp"

WingedVertex :: WingedVertex (unsigned int i, unsigned int l) 
  : _index (i), _level (l) {}

WingedVertex :: WingedVertex (unsigned int i, LinkedEdge e, unsigned int l) 
  : _index (i), _edge (e), _level (l) {}

void WingedVertex :: setEdge (LinkedEdge e) { this->_edge = e; }

void WingedVertex :: addIndex (WingedMesh& mesh) {
  mesh.addIndex (this->_index);
}

glm::vec3 WingedVertex :: vertex (const WingedMesh& mesh) const {
  return mesh.vertex (this->_index);
}

glm::vec3 WingedVertex :: normal (const WingedMesh& mesh) const {
  glm::vec3 normal = glm::vec3 (0.0f,0.0f,0.0f);

  for (ADJACENT_FACE_ITERATOR (it,*this)) {
    normal = normal + it.face ()->normal (mesh);
  }
  return glm::normalize (normal);
}

unsigned int WingedVertex :: valence () const {
  unsigned int i = 0;
  for (ADJACENT_EDGE_ITERATOR (it,*this))
    ++i;
  return i;
}

Maybe <LinkedEdge> WingedVertex :: tEdge () const {
  for (ADJACENT_EDGE_ITERATOR (it,*this)) {
    if (it.edge ()->isTEdge ())
      return Maybe <LinkedEdge> (it.edge ());
  }
  return Maybe <LinkedEdge> ();
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
AdjacentEdgeIterator WingedVertex :: adjacentEdgeIterator ( LinkedEdge e
                                                          , bool skipT) const {
  return AdjacentEdgeIterator (*this,e,skipT);
}
AdjacentVertexIterator WingedVertex :: adjacentVertexIterator ( LinkedEdge e
                                                              , bool skipT ) const {
  return AdjacentVertexIterator (*this,e,skipT);
}
AdjacentFaceIterator WingedVertex :: adjacentFaceIterator (LinkedEdge e) const {
  return AdjacentFaceIterator (*this,e);
}

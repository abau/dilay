#include <glm/glm.hpp>
#include "winged-vertex.hpp"
#include "winged-edge.hpp"
#include "winged-face.hpp"
#include "winged-mesh.hpp"
#include "winged-edge-iterator.hpp"
#include "triangle.hpp"

WingedFace :: WingedFace (LinkedEdge e) : _edge (e) {}

void WingedFace :: setEdge (LinkedEdge e) {
  this->_edge = e;
}

void WingedFace :: addIndices (WingedMesh& mesh) {
  for (WingedEdgeIterator it = this->edgeIterator (); it.hasEdge (); it.next ()) {
    it.edge ()->firstVertex (*this)->addIndex (mesh);
  }
}

void WingedFace :: triangle (const WingedMesh& mesh, Triangle& triangle) const {
  WingedEdge& e1 = *this->_edge;
  WingedEdge& e2 = *e1.successor (*this);

  triangle.vertex1 (e1.firstVertex  (*this)->vertex (mesh));
  triangle.vertex2 (e1.secondVertex (*this)->vertex (mesh));
  triangle.vertex3 (e2.secondVertex (*this)->vertex (mesh));
}

Triangle WingedFace :: triangle (const WingedMesh& mesh) const {
  Triangle t (glm::vec3 (0.0f),glm::vec3 (0.0f),glm::vec3 (0.0f));
  this->triangle (mesh,t);
  return t;
}

glm::vec3 WingedFace :: normal (const WingedMesh& mesh) const {
  glm::vec3 v1 = this->_edge->vertex (*this,0)->vertex (mesh);
  glm::vec3 v2 = this->_edge->vertex (*this,1)->vertex (mesh);
  glm::vec3 v3 = this->_edge->vertex (*this,2)->vertex (mesh);

  return glm::normalize (glm::cross (v2-v1, v3-v2));
}

WingedEdgeIterator WingedFace :: edgeIterator () const {
  return WingedEdgeIterator (*this);
}

unsigned int WingedFace :: numEdges () const {
  unsigned int i = 0;

  for (WINGED_EDGE_ITERATOR(_,*this)) {
    i++;
  }
  return i;
}

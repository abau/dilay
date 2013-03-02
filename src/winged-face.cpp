#include "winged-edge.hpp"
#include "winged-face.hpp"
#include "winged-mesh.hpp"

WingedFace :: WingedFace (LinkedEdge e) : _edge (e) {}

void WingedFace :: setEdge (LinkedEdge e) {
  this->_edge = e;
}

void WingedFace :: addIndices (WingedMesh& mesh) {
  for (WingedEdgeIterator it = this->edgeIterator (); it.hasEdge (); it.next ()) {
    it.edge ()->firstVertex (*this)->addIndex (mesh);
  }
}

Triangle WingedFace :: triangle (const WingedMesh& mesh) const {
  WingedEdge& e1 = *this->_edge;
  WingedEdge& e2 = *e1.successor (*this);
  
  return Triangle ( e1.firstVertex  (*this)->vertex (mesh)
                  , e1.secondVertex (*this)->vertex (mesh)
                  , e2.secondVertex (*this)->vertex (mesh)
      );
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

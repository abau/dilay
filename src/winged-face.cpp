#include "winged-face.hpp"
#include "winged-edge.hpp"

WingedFace :: WingedFace (LinkedEdge* e) {
  this->_edge = e;
}

void WingedFace :: setEdge (LinkedEdge* e) {
  this->_edge = e;
}

void WingedFace :: addIndices (WingedMesh& mesh) {
  for (WingedEdgeIterator it = this->edgeIterator (); it.hasEdge (); it.next ()) {
    it.edge ()->data ().firstVertex (*this)->data ().addIndex (mesh);
  }
}

Triangle WingedFace :: triangle (const WingedMesh& mesh) const {
  WingedEdge& e1 = this->_edge->data ();
  WingedEdge& e2 = e1.successor (*this)->data ();
  
  return Triangle ( e1.firstVertex  (*this)->data ().vertex (mesh)
                  , e1.secondVertex (*this)->data ().vertex (mesh)
                  , e2.secondVertex (*this)->data ().vertex (mesh)
      );
}

glm::vec3 WingedFace :: normal (const WingedMesh& mesh) const {
  glm::vec3 v1 = this->_edge->data ().vertex (*this,0)->data ().vertex (mesh);
  glm::vec3 v2 = this->_edge->data ().vertex (*this,1)->data ().vertex (mesh);
  glm::vec3 v3 = this->_edge->data ().vertex (*this,2)->data ().vertex (mesh);

  return glm::normalize (glm::cross (v2-v1, v3-v2));
}

WingedEdgeIterator WingedFace :: edgeIterator () const {
  return WingedEdgeIterator (*this);
}

#include "winged-face.hpp"
#include "winged-edge.hpp"

WingedFace :: WingedFace (LinkedEdge* e) {
  this->_edge = e;
}

void WingedFace :: setEdge (LinkedEdge* e) {
  this->_edge = e;
}

void WingedFace :: addIndices (WingedMesh& mesh) {
  WingedEdge& e1 = this->_edge->data ();
  WingedEdge& e2 = e1.successor (*this)->data ();
  
  e1.firstVertex  (*this)->data ().addIndex (mesh);
  e1.secondVertex (*this)->data ().addIndex (mesh);
  e2.secondVertex (*this)->data ().addIndex (mesh);
}

Triangle WingedFace :: triangle (const WingedMesh& mesh) const {
  WingedEdge& e1 = this->_edge->data ();
  WingedEdge& e2 = e1.successor (*this)->data ();
  
  return Triangle ( e1.firstVertex  (*this)->data ().vertex (mesh)
                  , e1.secondVertex (*this)->data ().vertex (mesh)
                  , e2.secondVertex (*this)->data ().vertex (mesh)
      );
}

LinkedEdge& WingedFace :: longestEdge (const WingedMesh& mesh) {
  LinkedEdge& e1 = *this->_edge->data ().successor (*this,0);
  LinkedEdge& e2 = *this->_edge->data ().successor (*this,1);
  LinkedEdge& e3 = *this->_edge->data ().successor (*this,2);

  float e1Length = e1.data ().lengthSqr (mesh);
  float e2Length = e2.data ().lengthSqr (mesh);
  float e3Length = e3.data ().lengthSqr (mesh);

  if (e1Length >= e2Length && e1Length >= e3Length)
    return e1;
  else if (e2Length >= e3Length)
    return e2;
  else
    return e3;
}

glm::vec3 WingedFace :: normal (const WingedMesh& mesh) const {
  glm::vec3 v1 = this->_edge->data ().vertex (*this,0)->data ().vertex (mesh);
  glm::vec3 v2 = this->_edge->data ().vertex (*this,1)->data ().vertex (mesh);
  glm::vec3 v3 = this->_edge->data ().vertex (*this,2)->data ().vertex (mesh);

  return glm::normalize (glm::cross (v2-v1, v3-v2));
}

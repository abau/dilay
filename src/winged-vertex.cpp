#include "winged-edge.hpp"
#include "winged-vertex.hpp"
#include "winged-mesh.hpp"

WingedVertex :: WingedVertex (unsigned int i, LinkedElement <WingedEdge>* e) 
  : _index (i) { this->edge  = e; }

void WingedVertex :: setEdge (LinkedElement <WingedEdge>* e) { this->edge = e; }

void WingedVertex :: addIndex (WingedMesh& mesh) {
  mesh.addIndex (this->_index);
}

glm::vec3 WingedVertex :: vertex (const WingedMesh& mesh) const {
  return mesh.vertex (this->_index);
}

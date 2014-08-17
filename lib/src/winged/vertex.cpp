#include <glm/glm.hpp>
#include "winged/mesh.hpp"
#include "winged/face.hpp"
#include "winged/edge.hpp"
#include "winged/vertex.hpp"
#include "adjacent-iterator.hpp"

WingedVertex :: WingedVertex (unsigned int i, WingedEdge* e) 
  : _index (i), _edge (e) {}

void WingedVertex :: edge (WingedEdge* e) { this->_edge = e; }

void WingedVertex :: writeIndex (WingedMesh& mesh, unsigned int faceIndex) {
  mesh.setIndex (faceIndex, this->_index);
}

glm::vec3 WingedVertex :: vector (const WingedMesh& mesh) const {
  return mesh.vector (this->_index);
}

glm::vec3 WingedVertex :: savedNormal (const WingedMesh& mesh) const {
  return mesh.normal (this->_index);
}

glm::vec3 WingedVertex :: interpolatedNormal (const WingedMesh& mesh) const {
  glm::vec3 normal = glm::vec3 (0.0f,0.0f,0.0f);

  for (WingedFace& f : this->adjacentFaces ()) {
    normal = normal + f.normal (mesh);
  }
  return glm::normalize (normal);
}

void WingedVertex :: writeNormal (WingedMesh& mesh, const glm::vec3& normal) {
  mesh.setNormal (this->_index, normal);
}

unsigned int WingedVertex :: valence () const {
  unsigned int i             = 0;
  AdjEdges     adjacentEdges = this->adjacentEdges ();
  for (auto it = adjacentEdges.begin (); it != adjacentEdges.end (); ++it) {
    ++i;
  }
  return i;
}

WingedEdge* WingedVertex :: tEdge () const {
  for (WingedEdge& edge : this->adjacentEdges ()) {
    if (edge.isTEdge ())
      return &edge;
  }
  return nullptr;
}

AdjEdges WingedVertex :: adjacentEdges (WingedEdge& e, bool skipT) const {
  return AdjEdges (*this, e, skipT);
}
AdjEdges WingedVertex :: adjacentEdges (bool skipT) const {
  return this->adjacentEdges (this->edgeRef (), skipT);
}
AdjVertices WingedVertex :: adjacentVertices (WingedEdge& e, bool skipT) const {
  return AdjVertices (*this, e, skipT);
}
AdjVertices WingedVertex :: adjacentVertices (bool skipT) const {
  return this->adjacentVertices (this->edgeRef (), skipT);
}
AdjFaces WingedVertex :: adjacentFaces (WingedEdge& e) const {
  return AdjFaces (*this, e);
}
AdjFaces WingedVertex :: adjacentFaces () const {
  return this->adjacentFaces (this->edgeRef ());
}

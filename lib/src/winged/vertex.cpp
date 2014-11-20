#include <glm/glm.hpp>
#include "adjacent-iterator.hpp"
#include "winged/edge.hpp"
#include "winged/face.hpp"
#include "winged/mesh.hpp"
#include "winged/vertex.hpp"

WingedVertex :: WingedVertex (unsigned int i) 
  : _index (i), _edge (nullptr) {}

void WingedVertex :: edge (WingedEdge* e) { this->_edge = e; }

void WingedVertex :: writeIndex (WingedMesh& mesh, unsigned int index) {
  mesh.setIndex (index, this->_index);
}

glm::vec3 WingedVertex :: position (const WingedMesh& mesh) const {
  return mesh.vector (this->_index);
}

glm::vec3 WingedVertex :: savedNormal (const WingedMesh& mesh) const {
  return mesh.normal (this->_index);
}

glm::vec3 WingedVertex :: interpolatedNormal (const WingedMesh& mesh) const {
  glm::vec3    normal = glm::vec3 (0.0f);
  unsigned int n      = 0;

  for (WingedFace& f : this->adjacentFaces ()) {
    if (f.isDegenerated (mesh) == false) {
      normal += f.normal (mesh);
      n++;
    }
  }
  assert (n > 0);
  return normal / float (n);
}

void WingedVertex :: writePosition (WingedMesh& mesh, const glm::vec3& pos) {
  mesh.setVertex (this->_index, pos);
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

AdjEdges WingedVertex :: adjacentEdges (WingedEdge& e) const {
  return AdjEdges (*this, e);
}
AdjEdges WingedVertex :: adjacentEdges () const {
  return this->adjacentEdges (this->edgeRef ());
}
AdjVertices WingedVertex :: adjacentVertices (WingedEdge& e) const {
  return AdjVertices (*this, e);
}
AdjVertices WingedVertex :: adjacentVertices () const {
  return this->adjacentVertices (this->edgeRef ());
}
AdjFaces WingedVertex :: adjacentFaces (WingedEdge& e) const {
  return AdjFaces (*this, e);
}
AdjFaces WingedVertex :: adjacentFaces () const {
  return this->adjacentFaces (this->edgeRef ());
}

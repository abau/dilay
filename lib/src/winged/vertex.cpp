/* This file is part of Dilay
 * Copyright © 2015,2016 Alexander Bau
 * Use and redistribute under the terms of the GNU General Public License
 */
#include "adjacent-iterator.hpp"
#include "primitive/triangle.hpp"
#include "winged/face.hpp"
#include "winged/mesh.hpp"
#include "winged/vertex.hpp"

WingedVertex :: WingedVertex (unsigned int i) 
  : _index    (i)
  , _edge     (nullptr) 
{}

bool WingedVertex::operator== (const WingedVertex& other) const {
  return this->_index == other._index;
}

bool WingedVertex::operator!= (const WingedVertex& other) const {
  return ! this->operator== (other);
}

void WingedVertex :: writeIndex (WingedMesh& mesh, unsigned int index) {
  mesh.index (index, this->_index);
}

const glm::vec3& WingedVertex :: position (const WingedMesh& mesh) const {
  return mesh.vector (this->_index);
}

const glm::vec3& WingedVertex :: savedNormal (const WingedMesh& mesh) const {
  return mesh.normal (this->_index);
}

glm::vec3 WingedVertex :: interpolatedNormal (const WingedMesh& mesh) const {
  glm::vec3    normal = glm::vec3 (0.0f);
  unsigned int n      = 0;

  for (WingedFace& f : this->adjacentFaces ()) {
    PrimTriangle triangle = f.triangle (mesh);
    if (triangle.isDegenerated () == false) {
      normal += triangle.normal ();
      n++;
    }
  }
  assert (n > 0);
  return normal / float (n);
}

bool WingedVertex :: isNewVertex (const WingedMesh& mesh) const {
  return mesh.isNewVertex (this->_index);
}

void WingedVertex :: position (WingedMesh& mesh, const glm::vec3& pos) {
  mesh.vertex (this->_index, pos);
}

void WingedVertex :: normal (WingedMesh& mesh, const glm::vec3& normal) {
  mesh.normal (this->_index, normal);
}

void WingedVertex :: setInterpolatedNormal (WingedMesh& mesh) {
  this->normal (mesh, this->interpolatedNormal (mesh));
}

void WingedVertex :: isNewVertex (WingedMesh& mesh, bool value) {
  mesh.isNewVertex (this->_index, value);
}

unsigned int WingedVertex :: valence () const {
  unsigned int i             = 0;
  AdjEdges     adjacentEdges = this->adjacentEdges ();
  for (auto it = adjacentEdges.begin (); it != adjacentEdges.end (); ++it) {
    ++i;
  }
  return i;
}

AdjEdges WingedVertex :: adjacentEdges () const {
  return AdjEdges (*this, this->edgeRef ());
}

AdjVertices WingedVertex :: adjacentVertices () const {
  return AdjVertices (*this, this->edgeRef ());
}

AdjFaces WingedVertex :: adjacentFaces () const {
  return AdjFaces (*this, this->edgeRef ());
}

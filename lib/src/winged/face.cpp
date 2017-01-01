/* This file is part of Dilay
 * Copyright © 2015,2016 Alexander Bau
 * Use and redistribute under the terms of the GNU General Public License
 */
#include "../util.hpp"
#include "adjacent-iterator.hpp"
#include "primitive/triangle.hpp"
#include "winged/edge.hpp"
#include "winged/face.hpp"
#include "winged/mesh.hpp"
#include "winged/vertex.hpp"

WingedFace :: WingedFace (unsigned int i)
  : _index     (i)
  , _edgeIndex (Util::invalidIndex ())
  , _edge      (nullptr)
  {}

bool WingedFace::operator== (const WingedFace& other) const {
  return this->_index == other._index;
}

bool WingedFace::operator!= (const WingedFace& other) const {
  return ! this->operator== (other);
}

void WingedFace :: edge (WingedEdge* e) {
  this->_edgeIndex = e ? e->index () : Util::invalidIndex ();
  this->_edge = e;
}

WingedVertex* WingedFace :: vertex (unsigned int index) const { 
  return this->_edge->vertex (*this, index);
}

void WingedFace :: writeIndices (WingedMesh& mesh) {
  assert (this->isTriangle ());

  this->vertex (0)->writeIndex (mesh, (3 * this->_index) + 0);
  this->vertex (1)->writeIndex (mesh, (3 * this->_index) + 1);
  this->vertex (2)->writeIndex (mesh, (3 * this->_index) + 2);
}

PrimTriangle WingedFace :: triangle (const WingedMesh& mesh) const {
  return PrimTriangle ( mesh.vector (mesh.index ((3 * this->_index) + 0))
                      , mesh.vector (mesh.index ((3 * this->_index) + 1))
                      , mesh.vector (mesh.index ((3 * this->_index) + 2)) );
}

void WingedFace :: triangle (const WingedMesh& mesh, glm::vec3& a, glm::vec3& b, glm::vec3& c) {
  a = mesh.vector (mesh.index ((3 * this->_index) + 0));
  b = mesh.vector (mesh.index ((3 * this->_index) + 1));
  c = mesh.vector (mesh.index ((3 * this->_index) + 2));
}

unsigned int WingedFace :: numEdges () const {
  unsigned int i             = 0;
  AdjEdges     adjacentEdges = this->adjacentEdges ();

  for (auto it = adjacentEdges.begin (); it != adjacentEdges.end (); ++it) {
    i++;
  }
  return i;
}

bool WingedFace :: isTriangle () const { return this->numEdges () == 3; }

AdjEdges WingedFace :: adjacentEdges () const {
  return AdjEdges (*this, this->edgeRef ());
}

AdjVertices WingedFace :: adjacentVertices () const {
  return AdjVertices (*this, this->edgeRef ());
}

AdjFaces WingedFace :: adjacentFaces () const {
  return AdjFaces (*this, this->edgeRef ());
}

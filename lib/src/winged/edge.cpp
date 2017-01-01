/* This file is part of Dilay
 * Copyright © 2015,2016 Alexander Bau
 * Use and redistribute under the terms of the GNU General Public License
 */
#include <glm/glm.hpp>
#include "../util.hpp"
#include "winged/edge.hpp"
#include "winged/face.hpp"
#include "winged/vertex.hpp"

WingedEdge :: WingedEdge (unsigned int i) 
  : _index                 (i)
  , _vertex1Index          (Util::invalidIndex ())
  , _vertex2Index          (Util::invalidIndex ())
  , _leftFaceIndex         (Util::invalidIndex ())
  , _rightFaceIndex        (Util::invalidIndex ())
  , _leftPredecessorIndex  (Util::invalidIndex ())
  , _leftSuccessorIndex    (Util::invalidIndex ())
  , _rightPredecessorIndex (Util::invalidIndex ())
  , _rightSuccessorIndex   (Util::invalidIndex ())
  , _vertex1               (nullptr)
  , _vertex2               (nullptr)
  , _leftFace              (nullptr)
  , _rightFace             (nullptr)
  , _leftPredecessor       (nullptr)
  , _leftSuccessor         (nullptr)
  , _rightPredecessor      (nullptr)
  , _rightSuccessor        (nullptr)
  {}

bool WingedEdge::operator== (const WingedEdge& other) const {
  return this->_index == other._index;
}

bool WingedEdge::operator!= (const WingedEdge& other) const {
  return ! this->operator== (other);
}

bool WingedEdge :: isLeftFace (const WingedFace& face) const {
  if (face == *this->_leftFace) {
    return true;
  }
  else if (face == *this->_rightFace) {
    return false;
  }
  else {
    DILAY_IMPOSSIBLE
  }
}

bool WingedEdge :: isRightFace (const WingedFace& face) const { 
  return ! this->isLeftFace (face); 
}

bool WingedEdge :: isVertex1 (const WingedVertex& vertex) const {
  if (vertex == *this->_vertex1) {
    return true;
  }
  else if (vertex == *this->_vertex2) {
    return false;
  }
  else {
    DILAY_IMPOSSIBLE
  }
}

bool WingedEdge :: isVertex2 (const WingedVertex& vertex) const { 
  return ! this->isVertex1 (vertex);
}

WingedVertex* WingedEdge :: firstVertex (const WingedFace& face) const {
  return this->isLeftFace (face) ? this->vertex1 () : this->vertex2 (); }

WingedVertex* WingedEdge :: secondVertex (const WingedFace& face) const {
  return this->isLeftFace (face) ? this->vertex2 () : this->vertex1 (); }

WingedEdge* WingedEdge :: predecessor (const WingedFace& face) const {
  return this->isLeftFace (face) ? this->leftPredecessor  () : this->rightPredecessor (); }

WingedEdge* WingedEdge :: successor (const WingedFace& face) const {
  return this->isLeftFace (face) ? this->leftSuccessor  () : this->rightSuccessor (); }

WingedFace* WingedEdge :: otherFace (const WingedFace& face) const {
  return this->isLeftFace (face) ? this->rightFace () : this->leftFace  (); }

WingedVertex* WingedEdge :: otherVertex (const WingedVertex& vertex) const {
  return this->isVertex1 (vertex) ? this->vertex2 () : this->vertex1 (); }

void WingedEdge :: vertex1 (WingedVertex* v) {
  this->_vertex1Index = v ? v->index () : Util::invalidIndex ();
  this->_vertex1 = v;
}

void WingedEdge :: vertex2 (WingedVertex* v) {
  this->_vertex2Index = v ? v->index () : Util::invalidIndex ();
  this->_vertex2 = v;
}

void WingedEdge :: leftFace (WingedFace* f) {
  this->_leftFaceIndex = f ? f->index () : Util::invalidIndex ();
  this->_leftFace = f;
}

void WingedEdge :: rightFace (WingedFace* f) {
  this->_rightFaceIndex = f ? f->index () : Util::invalidIndex ();
  this->_rightFace = f;
}

void WingedEdge :: leftPredecessor (WingedEdge* e) {
  this->_leftPredecessorIndex = e ? e->index () : Util::invalidIndex ();
  this->_leftPredecessor = e;
}

void WingedEdge :: leftSuccessor (WingedEdge* e) {
  this->_leftSuccessorIndex = e ? e->index () : Util::invalidIndex ();
  this->_leftSuccessor = e;
}

void WingedEdge :: rightPredecessor (WingedEdge* e) {
  this->_rightPredecessorIndex = e ? e->index () : Util::invalidIndex ();
  this->_rightPredecessor = e;
}

void WingedEdge :: rightSuccessor (WingedEdge* e) {
  this->_rightSuccessorIndex = e ? e->index () : Util::invalidIndex ();
  this->_rightSuccessor = e;
}

void WingedEdge :: setGeometry ( WingedVertex* v1, WingedVertex* v2
                               , WingedFace* left, WingedFace* right
                               , WingedEdge* leftPred , WingedEdge* leftSucc
                               , WingedEdge* rightPred, WingedEdge* rightSucc) {
  this->vertex1          (v1);
  this->vertex2          (v2);
  this->leftFace         (left);
  this->rightFace        (right);
  this->leftPredecessor  (leftPred);
  this->leftSuccessor    (leftSucc);
  this->rightPredecessor (rightPred);
  this->rightSuccessor   (rightSucc);
}

void WingedEdge :: firstVertex (const WingedFace& f, WingedVertex* v) {
  if (this->isLeftFace (f))
    this->vertex1 (v);
  else
    this->vertex2 (v);
}

void WingedEdge :: secondVertex (const WingedFace& f, WingedVertex* v) {
  if (this->isLeftFace (f))
    this->vertex2 (v);
  else
    this->vertex1 (v);
}

void WingedEdge :: vertex (const WingedVertex& vertex, WingedVertex* v) {
  if (this->isVertex1 (vertex))
    this->vertex1 (v);
  else 
    this->vertex2 (v);
}

void WingedEdge :: face (const WingedFace& fOld, WingedFace* fNew) {
  if (this->isLeftFace (fOld))
    this->leftFace (fNew);
  else
    this->rightFace (fNew);
}

void WingedEdge :: predecessor (const WingedFace& f, WingedEdge* e) {
  if (this->isLeftFace (f)) 
    this->leftPredecessor  (e);
  else 
    this->rightPredecessor (e);
}

void WingedEdge :: successor (const WingedFace& f, WingedEdge* e) {
  if (this->isLeftFace (f)) 
    this->leftSuccessor  (e);
  else 
    this->rightSuccessor (e);
}

glm::vec3 WingedEdge :: vector (const WingedMesh& mesh) const {
  glm::vec3 a = this->_vertex1->position (mesh);
  glm::vec3 b = this->_vertex2->position (mesh);
  return b-a;
}

glm::vec3 WingedEdge :: vector (const WingedMesh& mesh, const WingedVertex& vertex) const {
  return this->isVertex1 (vertex) ?  this->vector (mesh)
                                  : -this->vector (mesh);
}

float WingedEdge :: length (const WingedMesh& mesh) const {
  return glm::length (this->vector (mesh));
}

float WingedEdge :: lengthSqr (const WingedMesh& mesh) const {
  glm::vec3 v = this->vector (mesh);
  return glm::dot (v,v);
}

WingedEdge* WingedEdge :: successor (const WingedFace& face,unsigned int index) const {
  return index == 0 ? this->successor (face)
                    : this->successor (face)->successor (face,index-1);
}

WingedEdge* WingedEdge :: predecessor (const WingedFace& face,unsigned int index) const {
  return index == 0 ? this->predecessor (face)
                    : this->predecessor (face)->predecessor (face,index-1);
}

WingedVertex* WingedEdge :: vertex (const WingedFace& face,unsigned int index) const {
  return index == 0 ? this->firstVertex (face)
                    : this->successor (face)->vertex (face,index-1);
}

glm::vec3 WingedEdge :: middle (const WingedMesh& mesh) const {
  return Util :: between ( this->vertex1 ()->position (mesh)
                         , this->vertex2 ()->position (mesh));
}

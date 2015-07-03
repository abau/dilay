/* This file is part of Dilay
 * Copyright © 2015 Alexander Bau
 * Use and redistribute under the terms of the GNU General Public License
 */
#include <cassert>
#include "adjacent-iterator.hpp"
#include "winged/edge.hpp"
#include "winged/face.hpp"
#include "winged/mesh.hpp"
#include "winged/vertex.hpp"

AdjEdges::Iterator :: Iterator (const WingedFace& f, WingedEdge& e, bool i) 
  : _edge      (&e)
  , start      (&e)
  , isEnd      (i)
  , _face      (&f)
  , _vertex    (nullptr)
  {}

AdjEdges::Iterator :: Iterator (const WingedVertex& v, WingedEdge& e, bool i) 
  : _edge      (&e)
  , start      (&e)
  , isEnd      (i)
  , _face      (nullptr)
  , _vertex    (&v)
  {}

AdjEdges::Iterator& AdjEdges::Iterator::operator++ () { 
  if (this->_face) {
    this->_edge = this->_edge->successor (*this->_face);
  }
  else {
    this->_edge = this->_edge->isVertex1        (*this->_vertex) 
                ? this->_edge->leftPredecessor  ()
                : this->_edge->rightPredecessor ();
  }

  if (this->_edge == this->start) {
    this->isEnd = true;
  }
  return *this; 
}

bool AdjEdges::Iterator::operator!= (const AdjEdges::Iterator& o) const {
  return this->isEnd != o.isEnd || this->_edge != o._edge;
}

WingedEdge& AdjEdges::Iterator::operator* () const {
  return *this->operator-> ();
}

WingedEdge* AdjEdges::Iterator::operator-> () const {
  assert (this->isEnd == false);
  return this->_edge;
}

AdjEdges :: AdjEdges (const WingedFace& f, WingedEdge& e)
  : itBegin (f,e,false)
  , itEnd   (f,e,true)
  {}

AdjEdges :: AdjEdges (const WingedVertex& v, WingedEdge& e)
  : itBegin (v,e,false)
  , itEnd   (v,e,true)
  {}

std::vector <WingedEdge*> AdjEdges :: collect () {
  std::vector <WingedEdge*> v;
  for (WingedEdge& e : *this) {
    v.push_back (&e);
  }
  return v;
}

WingedVertex& AdjVertices::Iterator::operator* () const {
  return *this->operator-> ();
}

WingedVertex* AdjVertices::Iterator::operator-> () const {
  if (this->eIt.face ())
    return this->eIt->firstVertex (*this->eIt.face ());
  else
    return this->eIt->otherVertex (*this->eIt.vertex ());
}

AdjVertices :: AdjVertices (const WingedFace& f, WingedEdge& e) 
  : itBegin (f,e,false)
  , itEnd   (f,e,true)
  {}

AdjVertices :: AdjVertices (const WingedVertex& v, WingedEdge& e)
  : itBegin (v,e,false)
  , itEnd   (v,e,true)
  {}

std::vector <WingedVertex*> AdjVertices :: collect () {
  std::vector <WingedVertex*> v;
  for (WingedVertex& e : *this) {
    v.push_back (&e);
  }
  return v;
}

WingedFace& AdjFaces::Iterator::operator* () const {
  return *this->operator-> ();
}

WingedFace* AdjFaces::Iterator::operator-> () const {
  if (this->eIt.face ()) {
    return this->eIt->otherFace (*this->eIt.face ());
  }
  else if (this->eIt->isVertex1 (*this->eIt.vertex ())) {
    return this->eIt->rightFace ();
  }
  else {
    return this->eIt->leftFace ();
  }
}

AdjFaces :: AdjFaces (const WingedFace& f, WingedEdge& e) 
  : itBegin (f,e,false)
  , itEnd   (f,e,true)
  {}

AdjFaces :: AdjFaces (const WingedVertex& v, WingedEdge& e)
  : itBegin (v,e,false)
  , itEnd   (v,e,true)
  {}

std::vector <WingedFace*> AdjFaces :: collect () {
  std::vector <WingedFace*> v;
  for (WingedFace& e : *this) {
    v.push_back (&e);
  }
  return v;
}

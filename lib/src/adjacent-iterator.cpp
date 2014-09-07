#include <cassert>
#include "adjacent-iterator.hpp"
#include "id.hpp"
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

WingedEdge& AdjEdges::Iterator::operator* () const {
  assert (this->isEnd == false);
  return *this->_edge;
}

bool AdjEdges::Iterator::operator!= (const AdjEdges::Iterator& o) const {
  return this->isEnd != o.isEnd || this->_edge != o._edge;
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
  WingedEdge& edge = *this->eIt;
  if (this->eIt.face ())
    return edge.firstVertexRef (*this->eIt.face ());
  else
    return edge.otherVertexRef (*this->eIt.vertex ());
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
  WingedEdge& edge = *this->eIt;
  if (this->eIt.face ()) {
    return edge.otherFaceRef (*this->eIt.face ());
  }
  else if (edge.isVertex1 (*this->eIt.vertex ())) {
    return edge.rightFaceRef ();
  }
  else {
    return edge.leftFaceRef ();
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

#include "winged-mesh.hpp"
#include "winged-edge-iterator.hpp"

WingedEdgeIterator :: WingedEdgeIterator (const WingedFace& f) 
  : _face  (f)
  , _start (f.edge ())
  , _edge  (f.edge ())
  {}

bool WingedEdgeIterator :: hasEdge () const { 
  return this->_edge != 0; 
}

void WingedEdgeIterator :: next () {
  LinkedEdge* succ = this->_edge->data ().successor (this->_face);
  if (succ == this->_start)
    this->_edge = 0;
  else
    this->_edge = succ;
}

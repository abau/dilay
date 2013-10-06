#include <cassert>
#include "adjacent-iterator.hpp"
#include "winged-vertex.hpp"
#include "winged-edge.hpp"
#include "winged-face.hpp"
#include "winged-mesh.hpp"

AdjacentEdgeIterator :: AdjacentEdgeIterator (const WingedFace& f, bool skipT) 
  : AdjacentEdgeIterator (f, f.edgeRef (), skipT) {} 

AdjacentEdgeIterator :: AdjacentEdgeIterator ( const WingedFace& f, WingedEdge& e
                                             , bool skipT) 
  : _isValid     (true)
  , _face        (&f)
  , _vertex      (nullptr)
  , _start       (&e)
  , _edge        (&e)
  , _skipTEdges  (skipT) {

    if (this->_skipTEdges && this->element ().isTEdge ()) {
      this->next ();
      assert (! (this->_skipTEdges && this->element ().isTEdge ()));
      this->_start = &this->element ();
    }
  }

AdjacentEdgeIterator :: AdjacentEdgeIterator (const WingedVertex& v, bool skipT) 
  : AdjacentEdgeIterator (v, *v.edge (), skipT) {}

AdjacentEdgeIterator :: AdjacentEdgeIterator ( const WingedVertex& v, WingedEdge& e
                                             , bool skipT) 
  : _isValid     (true)
  , _face        (nullptr)
  , _vertex      (&v)
  , _start       (&e)
  , _edge        (&e)
  , _skipTEdges  (skipT) {
  
    while (this->_skipTEdges && this->isValid () && this->element ().isTEdge ()) {
      this->next ();
    }
  }

WingedEdge& AdjacentEdgeIterator :: element () const {
  assert (this->isValid ());
  return *this->_edge;
}

void AdjacentEdgeIterator :: next () {
  assert (this->isValid ());
  if (this->_face) {
    this->_edge = this->_edge->successor (*this->_face);

    if (this->_skipTEdges && this->_edge->isTEdge ()) {
      this->_face = this->_edge->otherFace (*this->_face);
      this->_edge = this->_edge->successor (*this->_face);
    }
  }
  else {
    do {
      this->_edge = this->_edge->isVertex1        (*this->_vertex) 
                  ? this->_edge->leftPredecessor  ()
                  : this->_edge->rightPredecessor ();
    } while (this->_skipTEdges && this->_edge->isTEdge ());
  }
  if (this->_edge == this->_start)
    this->_isValid = false;
}

std::list <WingedEdge*> AdjacentEdgeIterator :: collect () {
  std::list <WingedEdge*> l;
  while (this->isValid ()) {
    l.push_back (&this->element ());
    this->next ();
  }
  return l;
}

AdjacentVertexIterator :: AdjacentVertexIterator (const WingedFace& f, bool skipT)
  : _edgeIterator (f,skipT) {}
AdjacentVertexIterator :: AdjacentVertexIterator ( const WingedFace& f, WingedEdge& e
                                                 , bool skipT)
  : _edgeIterator (f,e,skipT) {}
AdjacentVertexIterator :: AdjacentVertexIterator (const WingedVertex& v, bool skipT)
  : _edgeIterator (v,skipT) {}
AdjacentVertexIterator :: AdjacentVertexIterator ( const WingedVertex& v, WingedEdge& e
                                                 , bool skipT)
  : _edgeIterator (v,e,skipT) {}

WingedVertex& AdjacentVertexIterator :: element () const {
  WingedEdge& edge = this->_edgeIterator.element ();
  if (this->_edgeIterator.face ())
    return edge.firstVertexRef (*this->_edgeIterator.face ());
  else
    return edge.otherVertexRef (*this->_edgeIterator.vertex ());
}

std::list <WingedVertex*> AdjacentVertexIterator :: collect () {
  std::list <WingedVertex*> l;
  while (this->isValid ()) {
    l.push_back (&this->element ());
    this->next ();
  }
  return l;
}

AdjacentFaceIterator :: AdjacentFaceIterator (const WingedFace& f, bool skipT)
  : _edgeIterator (f,skipT) {}
AdjacentFaceIterator :: AdjacentFaceIterator ( const WingedFace& f, WingedEdge& e
                                             , bool skipT)
  : _edgeIterator (f,e,skipT) {}
AdjacentFaceIterator :: AdjacentFaceIterator (const WingedVertex& v)
  : _edgeIterator (v) {}
AdjacentFaceIterator :: AdjacentFaceIterator (const WingedVertex& v, WingedEdge& e)
  : _edgeIterator (v,e) {}

WingedFace& AdjacentFaceIterator :: element () const {
  WingedEdge& edge = this->_edgeIterator.element ();
  if (this->_edgeIterator.face ()) {
    return edge.otherFaceRef (*this->_edgeIterator.face ());
  }
  else if (edge.isVertex1 (*this->_edgeIterator.vertex ())) {
    return edge.rightFaceRef ();
  }
  else {
    return edge.leftFaceRef ();
  }
}

std::list <WingedFace*> AdjacentFaceIterator :: collect () {
  std::list <WingedFace*> l;
  while (this->isValid ()) {
    l.push_back (&this->element ());
    this->next ();
  }
  return l;
}

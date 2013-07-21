#include <cassert>
#include "adjacent-iterator.hpp"
#include "winged-vertex.hpp"
#include "winged-edge.hpp"
#include "winged-face.hpp"
#include "winged-mesh.hpp"

AdjacentEdgeIterator :: AdjacentEdgeIterator (const WingedFace& f, bool skipT) 
  : AdjacentEdgeIterator (f, f.edge (), skipT) {}

AdjacentEdgeIterator :: AdjacentEdgeIterator ( const WingedFace& f, LinkedEdge e
                                             , bool skipT) 
  : _face        (&f)
  , _vertex      (nullptr)
  , _start       (e)
  , _edge        (e)
  , _hasEdge     (true)
  , _skipTEdges  (skipT) {

    if (this->_skipTEdges && this->element ()->isTEdge ()) {
      this->next ();
      assert (! (this->_skipTEdges && this->element ()->isTEdge ()));
      this->_start = this->element ();
    }
  }

AdjacentEdgeIterator :: AdjacentEdgeIterator (const WingedVertex& v, bool skipT) 
  : AdjacentEdgeIterator (v, v.edge (), skipT) {}

AdjacentEdgeIterator :: AdjacentEdgeIterator ( const WingedVertex& v, LinkedEdge e
                                             , bool skipT) 
  : _face        (nullptr)
  , _vertex      (&v)
  , _start       (e)
  , _edge        (e)
  , _hasEdge     (true)
  , _skipTEdges  (skipT) {
  
    while (this->_skipTEdges && this->isValid () && this->element ()->isTEdge ()) {
      this->next ();
    }
  }

LinkedEdge AdjacentEdgeIterator :: element () const {
  assert (this->_hasEdge);
  return this->_edge;
}

void AdjacentEdgeIterator :: next () {
  if (this->_face) {
    this->_edge = this->_edge->successor (*this->_face);

    if (this->_skipTEdges && this->_edge->isTEdge ()) {
      this->_face = &*this->_edge->otherFace (*this->_face);
      this->_edge =   this->_edge->successor (*this->_face);
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
    this->_hasEdge = false;
}

LinkedEdges AdjacentEdgeIterator :: collect () {
  LinkedEdges l;
  while (this->isValid ()) {
    l.push_back (this->element ());
    this->next ();
  }
  return l;
}

AdjacentVertexIterator :: AdjacentVertexIterator (const WingedFace& f, bool skipT)
  : _edgeIterator (f,skipT) {}
AdjacentVertexIterator :: AdjacentVertexIterator ( const WingedFace& f, LinkedEdge e
                                                 , bool skipT)
  : _edgeIterator (f,e,skipT) {}
AdjacentVertexIterator :: AdjacentVertexIterator (const WingedVertex& v, bool skipT)
  : _edgeIterator (v,skipT) {}
AdjacentVertexIterator :: AdjacentVertexIterator ( const WingedVertex& v, LinkedEdge e
                                                 , bool skipT)
  : _edgeIterator (v,e,skipT) {}

LinkedVertex AdjacentVertexIterator :: element () const {
  LinkedEdge edge = this->_edgeIterator.element ();
  if (this->_edgeIterator.face ())
    return edge->firstVertex (*this->_edgeIterator.face ());
  else
    return edge->otherVertex (*this->_edgeIterator.vertex ());
}

LinkedVertices AdjacentVertexIterator :: collect () {
  LinkedVertices l;
  while (this->isValid ()) {
    l.push_back (this->element ());
    this->next ();
  }
  return l;
}


AdjacentFaceIterator :: AdjacentFaceIterator (const WingedFace& f, bool skipT)
  : _edgeIterator (f,skipT) {}
AdjacentFaceIterator :: AdjacentFaceIterator ( const WingedFace& f, LinkedEdge e
                                             , bool skipT)
  : _edgeIterator (f,e,skipT) {}
AdjacentFaceIterator :: AdjacentFaceIterator (const WingedVertex& v)
  : _edgeIterator (v) {}
AdjacentFaceIterator :: AdjacentFaceIterator (const WingedVertex& v, LinkedEdge e)
  : _edgeIterator (v,e) {}

LinkedFace AdjacentFaceIterator :: element () const {
  LinkedEdge edge = this->_edgeIterator.element ();
  if (this->_edgeIterator.face ()) {
    return edge->otherFace (*this->_edgeIterator.face ());
  }
  else if (edge->isVertex1 (*this->_edgeIterator.vertex ())) {
    return edge->rightFace ();
  }
  else {
    return edge->leftFace ();
  }
}

LinkedFaces AdjacentFaceIterator :: collect () {
  LinkedFaces l;
  while (this->isValid ()) {
    l.push_back (this->element ());
    this->next ();
  }
  return l;
}


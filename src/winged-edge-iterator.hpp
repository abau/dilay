#ifndef WINGED_EDGE_ITERATOR
#define WINGED_EDGE_ITERATOR

#include "util.hpp"

class WingedEdgeIterator {
  public:
                WingedEdgeIterator (const WingedFace& f); 

    LinkedEdge  edge               (); 
    bool        hasEdge            () const { return this->_hasEdge; }
    void        next               ();     

  private:
    const WingedFace& _face;
    const LinkedEdge  _start;
          LinkedEdge  _edge;
          bool        _hasEdge;
};
#endif

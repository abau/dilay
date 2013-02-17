#ifndef WINGED_EDGE_ITERATOR
#define WINGED_EDGE_ITERATOR

class WingedFace;

class WingedEdgeIterator {
  public:
                WingedEdgeIterator (const WingedFace& f); 

    LinkedEdge* edge               () const { return this->_edge; }
    bool        hasEdge            () const;
    void        next               ();     

  private:
    const WingedFace&       _face;
          LinkedEdge* const _start;
          LinkedEdge*       _edge;
};
#endif

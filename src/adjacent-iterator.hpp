#ifndef DILAY_ADJACENT_ITERATOR
#define DILAY_ADJACENT_ITERATOR

#include "iterator.hpp"

#define ADJACENT_EDGE_ITERATOR(it,x)   AdjacentEdgeIterator   it = (x).adjacentEdgeIterator ()   ; it.isValid (); it.next ()
#define ADJACENT_VERTEX_ITERATOR(it,x) AdjacentVertexIterator it = (x).adjacentVertexIterator () ; it.isValid (); it.next ()
#define ADJACENT_FACE_ITERATOR(it,x)   AdjacentFaceIterator   it = (x).adjacentFaceIterator ()   ; it.isValid (); it.next ()

class WingedVertex;
class WingedEdge;
class WingedFace;

/** Iterates over adjacent edges of a face or vertex in counterclockwise order */
class AdjacentEdgeIterator : public Iterator <WingedEdge> {
  public: AdjacentEdgeIterator  (const WingedFace&, bool = false); 
          /** It's crucial that the edge is adjacent to the face */
          AdjacentEdgeIterator  (const WingedFace&, WingedEdge&, bool = false); 
          AdjacentEdgeIterator  (const WingedVertex&, bool = false);
          /** It's crucial that the edge is adjacent to the vertex */
          AdjacentEdgeIterator  (const WingedVertex&, WingedEdge&, bool = false);

    WingedEdge&         element () const; 
    const WingedFace*   face    () const { return this->_face; }
    const WingedVertex* vertex  () const { return this->_vertex; }
    bool                isValid () const { return this->_isValid; }
    void                next    ();     

  private:
          bool          _isValid;
    const WingedFace*   _face;
    const WingedVertex* _vertex;
          WingedEdge*   _start;
          WingedEdge*   _edge;
          bool          _skipTEdges;
};

/** Iterates over adjacent vertices of a face or vertex in counterclockwise order */
class AdjacentVertexIterator : public Iterator <WingedVertex> {
  public: AdjacentVertexIterator (const WingedFace&, bool = false); 
          /** It's crucial that the edge is adjacent to the face */
          AdjacentVertexIterator (const WingedFace&, WingedEdge&, bool = false); 
          AdjacentVertexIterator (const WingedVertex&, bool = false); 
          /** It's crucial that the edge is adjacent to the vertex */
          AdjacentVertexIterator (const WingedVertex&, WingedEdge&, bool = false); 

    WingedVertex&  element () const;
    bool           isValid () const { return this->_edgeIterator.isValid (); }
    void           next    ()       { this->_edgeIterator.next (); }

  private:
    AdjacentEdgeIterator _edgeIterator;
};

/** Iterates over adjacent faces of a face or vertex in counterclockwise order */
class AdjacentFaceIterator : public Iterator <WingedFace> {
  public: AdjacentFaceIterator (const WingedFace&, bool = false); 
          /** It's crucial that the edge is adjacent to the face */
          AdjacentFaceIterator (const WingedFace&, WingedEdge&, bool = false); 
          AdjacentFaceIterator (const WingedVertex&); 
          /** It's crucial that the edge is adjacent to the vertex */
          AdjacentFaceIterator (const WingedVertex&, WingedEdge&); 

    WingedFace&  element () const;
    bool         isValid () const { return this->_edgeIterator.isValid (); }
    void         next    ()       { this->_edgeIterator.next (); }

  private:
    AdjacentEdgeIterator _edgeIterator;
};
#endif

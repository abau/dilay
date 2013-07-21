#ifndef DILAY_ADJACENT_ITERATOR
#define DILAY_ADJACENT_ITERATOR

#include "fwd-winged.hpp"
#include "iterator.hpp"

#define ADJACENT_EDGE_ITERATOR(it,x)   AdjacentEdgeIterator   it = (x).adjacentEdgeIterator ()   ; it.isValid (); it.next ()
#define ADJACENT_VERTEX_ITERATOR(it,x) AdjacentVertexIterator it = (x).adjacentVertexIterator () ; it.isValid (); it.next ()
#define ADJACENT_FACE_ITERATOR(it,x)   AdjacentFaceIterator   it = (x).adjacentFaceIterator ()   ; it.isValid (); it.next ()

/** Iterates over adjacent edges of a face or vertex in counterclockwise order */
class AdjacentEdgeIterator : public Iterator <LinkedEdge> {
  public: AdjacentEdgeIterator  (const WingedFace&, bool = false); 
          /** It's crucial that the edge is adjacent to the face */
          AdjacentEdgeIterator  (const WingedFace&, LinkedEdge, bool = false); 
          AdjacentEdgeIterator  (const WingedVertex&, bool = false);
          /** It's crucial that the edge is adjacent to the vertex */
          AdjacentEdgeIterator  (const WingedVertex&, LinkedEdge, bool = false);

    LinkedEdge          element () const; 
    const WingedFace*   face    () const { return this->_face; }
    const WingedVertex* vertex  () const { return this->_vertex; }
    bool                isValid () const { return this->_hasEdge; }
    void                next    ();     
    LinkedEdges         collect ();

  private:
    const WingedFace*   _face;
    const WingedVertex* _vertex;
          LinkedEdge    _start;
          LinkedEdge    _edge;
          bool          _hasEdge;
          bool          _skipTEdges;
};

/** Iterates over adjacent vertices of a face or vertex in counterclockwise order */
class AdjacentVertexIterator : public Iterator <LinkedVertex> {
  public: AdjacentVertexIterator (const WingedFace&, bool = false); 
          /** It's crucial that the edge is adjacent to the face */
          AdjacentVertexIterator (const WingedFace&, LinkedEdge, bool = false); 
          AdjacentVertexIterator (const WingedVertex&, bool = false); 
          /** It's crucial that the edge is adjacent to the vertex */
          AdjacentVertexIterator (const WingedVertex&, LinkedEdge, bool = false); 

    LinkedVertex   element () const;
    bool           isValid () const { return this->_edgeIterator.isValid (); }
    void           next    ()       { this->_edgeIterator.next (); }
    LinkedVertices collect ();

  private:
    AdjacentEdgeIterator _edgeIterator;
};

/** Iterates over adjacent faces of a face or vertex in counterclockwise order */
class AdjacentFaceIterator : public Iterator <LinkedFace> {
  public: AdjacentFaceIterator (const WingedFace&, bool = false); 
          /** It's crucial that the edge is adjacent to the face */
          AdjacentFaceIterator (const WingedFace&, LinkedEdge, bool = false); 
          AdjacentFaceIterator (const WingedVertex&); 
          /** It's crucial that the edge is adjacent to the vertex */
          AdjacentFaceIterator (const WingedVertex&, LinkedEdge); 

    LinkedFace   element () const;
    bool         isValid () const { return this->_edgeIterator.isValid (); }
    void         next    ()       { this->_edgeIterator.next (); }
    LinkedFaces  collect ();

  private:
    AdjacentEdgeIterator _edgeIterator;
};
#endif

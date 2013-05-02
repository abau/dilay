#ifndef DILAY_ADJACENT_ITERATOR
#define DILAY_ADJACENT_ITERATOR

#include "fwd-winged.hpp"

#define ADJACENT_EDGE_ITERATOR(it,x)   AdjacentEdgeIterator   it = (x).adjacentEdgeIterator ()   ; it.hasEdge ()  ; it.next ()
#define ADJACENT_VERTEX_ITERATOR(it,x) AdjacentVertexIterator it = (x).adjacentVertexIterator () ; it.hasVertex (); it.next ()
#define ADJACENT_FACE_ITERATOR(it,x)   AdjacentFaceIterator   it = (x).adjacentFaceIterator ()   ; it.hasFace ()  ; it.next ()

/** Iterates over adjacent edges of a face or vertex in counterclockwise order */
class AdjacentEdgeIterator {
  public: AdjacentEdgeIterator  (const WingedFace&, bool = false); 
          /** It's crucial that the edge is adjacent to the face */
          AdjacentEdgeIterator  (const WingedFace&, LinkedEdge, bool = false); 
          AdjacentEdgeIterator  (const WingedVertex&, bool = false);
          /** It's crucial that the edge is adjacent to the vertex */
          AdjacentEdgeIterator  (const WingedVertex&, LinkedEdge, bool = false);

    LinkedEdge          edge    () const; 
    const WingedFace*   face    () const { return this->_face; }
    const WingedVertex* vertex  () const { return this->_vertex; }
    bool                hasEdge () const { return this->_hasEdge; }
    void                next    ();     
    EdgeList            collect ();

  private:
    const WingedFace*   _face;
    const WingedVertex* _vertex;
          LinkedEdge    _start;
          LinkedEdge    _edge;
          bool          _hasEdge;
          bool          _skipTEdges;
};

/** Iterates over adjacent vertices of a face or vertex in counterclockwise order */
class AdjacentVertexIterator {
  public: AdjacentVertexIterator (const WingedFace&, bool = false); 
          /** It's crucial that the edge is adjacent to the face */
          AdjacentVertexIterator (const WingedFace&, LinkedEdge, bool = false); 
          AdjacentVertexIterator (const WingedVertex&, bool = false); 
          /** It's crucial that the edge is adjacent to the vertex */
          AdjacentVertexIterator (const WingedVertex&, LinkedEdge, bool = false); 

    LinkedVertex vertex          () const;
    bool         hasVertex       () const { return this->_edgeIterator.hasEdge (); }
    void         next            ()       { this->_edgeIterator.next (); }
    VertexList   collect         ();

  private:
    AdjacentEdgeIterator _edgeIterator;
};

/** Iterates over adjacent faces of a face or vertex in counterclockwise order */
class AdjacentFaceIterator {
  public: AdjacentFaceIterator (const WingedFace&, bool = false); 
          /** It's crucial that the edge is adjacent to the face */
          AdjacentFaceIterator (const WingedFace&, LinkedEdge, bool = false); 
          AdjacentFaceIterator (const WingedVertex&); 
          /** It's crucial that the edge is adjacent to the vertex */
          AdjacentFaceIterator (const WingedVertex&, LinkedEdge); 

    LinkedFace   face            () const;
    bool         hasFace         () const { return this->_edgeIterator.hasEdge (); }
    void         next            ()       { this->_edgeIterator.next (); }
    FaceList     collect         ();

  private:
    AdjacentEdgeIterator _edgeIterator;
};
#endif

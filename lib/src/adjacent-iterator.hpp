#ifndef DILAY_ADJACENT_ITERATOR
#define DILAY_ADJACENT_ITERATOR

#include <vector>

class WingedVertex;
class WingedEdge;
class WingedFace;
class Id;

/** Iterates over adjacent edges of a face or vertex in counterclockwise order */
class AdjEdges {
  public:
    class Iterator {
      public:
        Iterator (const WingedFace&  , WingedEdge&, bool);
        Iterator (const WingedVertex&, WingedEdge&, bool);

        Iterator&   operator++ ();
        bool        operator!= (const Iterator&) const;
        WingedEdge& operator*  ()                const;

              WingedEdge*   edge   () const { return this->_edge;   }
        const WingedFace*   face   () const { return this->_face;   }
        const WingedVertex* vertex () const { return this->_vertex; }

      private:
              WingedEdge*   _edge;
              WingedEdge*   start;
              bool          isEnd;
        const WingedFace*   _face;
        const WingedVertex* _vertex;
    };

    /** It's crucial that the edge is adjacent to the face */
    AdjEdges (const WingedFace&  , WingedEdge&);
    /** It's crucial that the edge is adjacent to the vertex */
    AdjEdges (const WingedVertex&, WingedEdge&);

    Iterator begin () const { return this->itBegin; }
    Iterator end   () const { return this->itEnd  ; }

    std::vector <WingedEdge*> collect ();

  private:
    const Iterator itBegin;
    const Iterator itEnd;
};

/** Iterates over adjacent vertices of a face or vertex in counterclockwise order */
class AdjVertices {
  public:
    class Iterator {
      public:
        Iterator (const WingedFace&   f, WingedEdge& e, bool i) : eIt (f,e,i) {}
        Iterator (const WingedVertex& v, WingedEdge& e, bool i) : eIt (v,e,i) {}

        Iterator&     operator++ ()                        { ++this->eIt; return *this; }
        bool          operator!= (const Iterator& o) const { return this->eIt != o.eIt; }
        WingedVertex& operator*  ()                  const;

              WingedEdge*   edge   () const { return this->eIt.edge   ();  }
        const WingedFace*   face   () const { return this->eIt.face   ();  }
        const WingedVertex* vertex () const { return this->eIt.vertex ();  }

      private:
        AdjEdges::Iterator eIt;
    };

    /** It's crucial that the edge is adjacent to the face */
    AdjVertices (const WingedFace&  , WingedEdge&); 
    /** It's crucial that the edge is adjacent to the vertex */
    AdjVertices (const WingedVertex&, WingedEdge&); 

    Iterator begin () const { return this->itBegin; }
    Iterator end   () const { return this->itEnd  ; }

    std::vector <WingedVertex*> collect ();

  private:
    const Iterator itBegin;
    const Iterator itEnd;
};

/** Iterates over adjacent faces of a face or vertex in counterclockwise order */
class AdjFaces {
  public:
    class Iterator {
      public:
        Iterator (const WingedFace&   f, WingedEdge& e, bool i) : eIt (f,e,i) {}
        Iterator (const WingedVertex& v, WingedEdge& e, bool i) : eIt (v,e,i) {}

        Iterator&     operator++ ()                        { ++this->eIt; return *this; }
        bool          operator!= (const Iterator& o) const { return this->eIt != o.eIt; }
        WingedFace&   operator*  ()                  const;

              WingedEdge*   edge   () const { return this->eIt.edge   ();  }
        const WingedFace*   face   () const { return this->eIt.face   ();  }
        const WingedVertex* vertex () const { return this->eIt.vertex ();  }

      private:
        AdjEdges::Iterator eIt;
    };

    /** It's crucial that the edge is adjacent to the face */
    AdjFaces (const WingedFace&  , WingedEdge&); 
    /** It's crucial that the edge is adjacent to the vertex */
    AdjFaces (const WingedVertex&, WingedEdge&); 

    Iterator begin () const { return this->itBegin; }
    Iterator end   () const { return this->itEnd  ; }

    std::vector <WingedFace*> collect ();

  private:
    const Iterator itBegin;
    const Iterator itEnd;
};
#endif

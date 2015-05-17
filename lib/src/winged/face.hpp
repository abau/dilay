#ifndef DILAY_WINGED_FACE
#define DILAY_WINGED_FACE

#include "macro.hpp"

class AdjEdges;
class AdjVertices;
class AdjFaces;
class PrimTriangle;
class WingedEdge;
class WingedVertex;
class WingedMesh;

class WingedFace {
  public:                      
    WingedFace (unsigned int);
    WingedFace (const WingedFace&)  = default;
    WingedFace (      WingedFace&&) = default;

    bool          operator==       (const WingedFace&) const;
    bool          operator!=       (const WingedFace&) const;

    unsigned int  index            () const { return this->_index; }
    WingedEdge*   edge             () const { return this->_edge; }

    void          edge             (WingedEdge*  e) { this->_edge = e; }

    WingedVertex* vertex           (unsigned int) const;
    void          writeIndices     (WingedMesh&);
    PrimTriangle  triangle         (const WingedMesh&) const;
    unsigned int  numEdges         () const;
    bool          isTriangle       () const;

    AdjEdges      adjacentEdges    (WingedEdge&) const;
    AdjEdges      adjacentEdges    ()            const;
    AdjVertices   adjacentVertices (WingedEdge&) const;
    AdjVertices   adjacentVertices ()            const;
    AdjFaces      adjacentFaces    (WingedEdge&) const;
    AdjFaces      adjacentFaces    ()            const;

    SAFE_REF_CONST  (WingedEdge  , edge)
    SAFE_REF1_CONST (WingedVertex, vertex, unsigned int)
  private:
    const unsigned int _index;
    WingedEdge*        _edge;
};

#endif

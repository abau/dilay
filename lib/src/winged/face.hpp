#ifndef DILAY_WINGED_FACE
#define DILAY_WINGED_FACE

#include "macro.hpp"

class AdjEdges;
class AdjVertices;
class AdjFaces;
class PrimTriangle;
class OctreeNode;
class WingedEdge;
class WingedVertex;
class WingedMesh;

class WingedFace {
  public:                      
    WingedFace (unsigned int);
    WingedFace (const WingedFace&)  = default;
    WingedFace (      WingedFace&&) = default;

    unsigned int           index            () const { return this->_index; }
    WingedEdge*            edge             () const { return this->_edge; }
    OctreeNode*            octreeNode       () const { return this->_octreeNode; }

    void                   edge             (WingedEdge*  e) { this->_edge       = e; }
    void                   octreeNode       (OctreeNode*  n) { this->_octreeNode = n; }

    WingedVertex*          vertex           (unsigned int) const;
    void                   writeIndices     (WingedMesh&);
    PrimTriangle           triangle         (const WingedMesh&) const;
    unsigned int           numEdges         () const;
    bool                   isTriangle       () const;

    AdjEdges               adjacentEdges    (WingedEdge&) const;
    AdjEdges               adjacentEdges    ()            const;
    AdjVertices            adjacentVertices (WingedEdge&) const;
    AdjVertices            adjacentVertices ()            const;
    AdjFaces               adjacentFaces    (WingedEdge&) const;
    AdjFaces               adjacentFaces    ()            const;

    SAFE_REF_CONST  (WingedEdge  , edge)
    SAFE_REF_CONST  (OctreeNode  , octreeNode)
    SAFE_REF1_CONST (WingedVertex, vertex, unsigned int)
  private:
    const unsigned int _index;
    WingedEdge*        _edge;
    OctreeNode*        _octreeNode;
};

#endif

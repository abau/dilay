#ifndef DILAY_WINGED_FACE
#define DILAY_WINGED_FACE

#include <glm/fwd.hpp>
#include "fwd-winged.hpp"
#include "macro.hpp"
#include "id.hpp"

class AdjacentEdgeIterator;
class AdjacentVertexIterator;
class AdjacentFaceIterator;
class PrimTriangle;
class OctreeNode;
class WingedEdge;
class WingedVertex;

class WingedFace {
  public:                      
    WingedFace ();
    WingedFace (WingedEdge*, const Id&, OctreeNode* = nullptr, unsigned int = 0);
    WingedFace (const WingedFace&)  = default;
    WingedFace (      WingedFace&&) = default;

    Id                     id               () const { return this->_id.id (); }
    WingedEdge*            edge             () const { return this->_edge; }
    OctreeNode*            octreeNode       () const { return this->_octreeNode; }
    Faces::iterator        iterator         () const { return this->_iterator; }
    unsigned int           firstIndexNumber () const { return this->_firstIndexNumber; }

    void                   edge            (WingedEdge* e) { this->_edge       = e; }
    void                   octreeNode      (OctreeNode* n) { this->_octreeNode = n; }
    void                   iterator        (const Faces::iterator& i) 
                                                           { this->_iterator = i; }
    void                   writeIndices    (WingedMesh&, const unsigned int* = nullptr);
    void                   writeNormals    (WingedMesh&);
    void                   write           (WingedMesh&, const unsigned int* = nullptr);
    PrimTriangle           triangle        (const WingedMesh&) const;
    WingedVertex&          firstVertex     () const;
    WingedVertex&          secondVertex    () const;
    WingedVertex&          thirdVertex     () const;
    unsigned int           numEdges        () const;
    glm::vec3              normal          (const WingedMesh&) const;

    /** `adjacent (v,b)` returns an adjacent edge with `v` as start or end point.
     * It's crucial that `v` is actually a vertex of `this` face.
     * If `b` holds, then t-edges are skiped.
     */
    WingedEdge*            adjacent        (const WingedVertex&, bool = false) const;
    WingedEdge*            longestEdge     (const WingedMesh&) const;
    WingedVertex*          tVertex         () const;
    WingedEdge*            tEdge           () const;
    bool                   isTriangle      () const;
    float                  incircleRadius  (const WingedMesh&) const;

    WingedVertex*          designatedTVertex      () const;
    AdjacentEdgeIterator   adjacentEdgeIterator   (bool = false) const;
    AdjacentVertexIterator adjacentVertexIterator (bool = false) const;
    AdjacentFaceIterator   adjacentFaceIterator   (bool = false) const;
    AdjacentEdgeIterator   adjacentEdgeIterator   (WingedEdge&, bool = false) const;
    AdjacentVertexIterator adjacentVertexIterator (WingedEdge&, bool = false) const;
    AdjacentFaceIterator   adjacentFaceIterator   (WingedEdge&, bool = false) const;

    SAFE_REF_CONST  (WingedEdge, edge)
    SAFE_REF_CONST  (OctreeNode, octreeNode)
    SAFE_REF1_CONST (WingedEdge, adjacent, const WingedVertex&)
    SAFE_REF2_CONST (WingedEdge, adjacent, const WingedVertex&, bool)
    SAFE_REF1_CONST (WingedEdge, longestEdge, const WingedMesh&)
    SAFE_REF_CONST  (WingedVertex, tVertex)
    SAFE_REF_CONST  (WingedEdge, tEdge)
    SAFE_REF_CONST  (WingedVertex, designatedTVertex)
  private:
    const IdObject  _id;
    WingedEdge*     _edge;
    OctreeNode*     _octreeNode;
    Faces::iterator _iterator;
    unsigned int    _firstIndexNumber;
};

#endif

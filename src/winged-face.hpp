#ifndef DILAY_WINGED_FACE
#define DILAY_WINGED_FACE

#include "fwd-winged.hpp"
#include "fwd-glm.hpp"
#include "macro.hpp"
#include "id.hpp"
#include "maybe.hpp"

class AdjacentEdgeIterator;
class AdjacentVertexIterator;
class AdjacentFaceIterator;
class Triangle;
class OctreeNode;
class WingedEdge;
class WingedVertex;

class WingedFace {
  public:                      
    typedef Maybe <unsigned int> FirstIndexNumber;
    
    WingedFace ( WingedEdge* = nullptr 
               , const Id& = Id ()
               , OctreeNode* = nullptr
               , const FirstIndexNumber& = FirstIndexNumber ()
               );
    Id                         id               () const { return this->_id.id (); }
    WingedEdge*                edge             () const { return this->_edge; }
    OctreeNode*                octreeNode       () const { return this->_octreeNode; }
    Faces::iterator            iterator         () const { return this->_iterator; }
    const Maybe<unsigned int>& firstIndexNumber () const { return this->_firstIndexNumber; }

    void                   edge            (WingedEdge* e) { this->_edge       = e; }
    void                   octreeNode      (OctreeNode* n) { this->_octreeNode = n; }
    void                   iterator        (const Faces::iterator& i) 
                                                           { this->_iterator = i; }
    void                   writeIndices    (WingedMesh&);
    void                   writeNormals    (WingedMesh&);
    void                   write           (WingedMesh&);
    Triangle               triangle        (const WingedMesh&) const;
    unsigned int           numEdges        () const;
    glm::vec3              normal          (const WingedMesh&) const;

    /** `adjacent (v)` returns an adjacent edge with `v` as start or end point.
     * It's crucial that `v` is acutally a vertex of `this` face
     */
    WingedEdge*            adjacent        (const WingedVertex&) const;
    WingedEdge*            longestEdge     (const WingedMesh&) const;
    WingedVertex*          tVertex         () const;
    WingedEdge*            tEdge           () const;
    unsigned int           level           () const;
    bool                   isTriangle      () const;

    WingedVertex*          highestLevelVertex     () const;
    void                   resetFirstIndexNumber  ();

    AdjacentEdgeIterator   adjacentEdgeIterator   (bool = false) const;
    AdjacentVertexIterator adjacentVertexIterator (bool = false) const;
    AdjacentFaceIterator   adjacentFaceIterator   (bool = false) const;
    AdjacentEdgeIterator   adjacentEdgeIterator   (WingedEdge&, bool = false) const;
    AdjacentVertexIterator adjacentVertexIterator (WingedEdge&, bool = false) const;
    AdjacentFaceIterator   adjacentFaceIterator   (WingedEdge&, bool = false) const;

    SAFE_REF  (WingedEdge, edge)
    SAFE_REF  (OctreeNode, octreeNode)
    SAFE_REF1 (WingedEdge, adjacent, const WingedVertex&)
    SAFE_REF1 (WingedEdge, longestEdge, const WingedMesh&)
    SAFE_REF  (WingedVertex, tVertex)
    SAFE_REF  (WingedEdge, tEdge)
    SAFE_REF  (WingedVertex, highestLevelVertex)
  private:
    const IdObject   _id;
    WingedEdge*      _edge;
    OctreeNode*      _octreeNode;
    Faces::iterator  _iterator;
    FirstIndexNumber _firstIndexNumber;
};

#endif

#ifndef DILAY_WINGED_FACE
#define DILAY_WINGED_FACE

#include <glm/fwd.hpp>
#include "macro.hpp"
#include "id.hpp"

class AdjEdges;
class AdjVertices;
class AdjFaces;
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

    const Id&              id              () const { return this->_id.id (); }
    WingedEdge*            edge            () const { return this->_edge; }
    OctreeNode*            octreeNode      () const { return this->_octreeNode; }
    unsigned int           index           () const { return this->_index; }

    void                   edge            (WingedEdge* e) { this->_edge       = e; }
    void                   octreeNode      (OctreeNode* n) { this->_octreeNode = n; }
    void                   writeIndices    (WingedMesh&, const unsigned int* = nullptr);
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
    WingedEdge*            adjacent               (const WingedVertex&, bool = false) const;
    WingedEdge*            longestEdge            (const WingedMesh&, float*) const;
    float                  longestEdgeLengthSqr   (const WingedMesh&) const;
    WingedVertex*          tVertex                () const;
    WingedEdge*            tEdge                  () const;
    bool                   isTriangle             () const;

    WingedVertex*          designatedTVertex      () const;
    AdjEdges               adjacentEdges          (WingedEdge&, bool = false) const;
    AdjEdges               adjacentEdges          (bool = false)              const;
    AdjVertices            adjacentVertices       (WingedEdge&, bool = false) const;
    AdjVertices            adjacentVertices       (bool = false)              const;
    AdjFaces               adjacentFaces          (WingedEdge&, bool = false) const;
    AdjFaces               adjacentFaces          (bool = false)              const;

    SAFE_REF_CONST  (WingedEdge, edge)
    SAFE_REF_CONST  (OctreeNode, octreeNode)
    SAFE_REF1_CONST (WingedEdge, adjacent, const WingedVertex&)
    SAFE_REF2_CONST (WingedEdge, adjacent, const WingedVertex&, bool)
    SAFE_REF2_CONST (WingedEdge, longestEdge, const WingedMesh&, float*)
    SAFE_REF_CONST  (WingedVertex, tVertex)
    SAFE_REF_CONST  (WingedEdge, tEdge)
    SAFE_REF_CONST  (WingedVertex, designatedTVertex)
  private:
    const IdObject  _id;
    WingedEdge*     _edge;
    OctreeNode*     _octreeNode;
    unsigned int    _index;
};

#endif

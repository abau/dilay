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
class WingedMesh;

class WingedFace {
  public:                      
    WingedFace ();
    WingedFace (const Id&, WingedEdge*);
    WingedFace (const Id&, WingedEdge*, OctreeNode*, unsigned int);
    WingedFace (const WingedFace&)  = default;
    WingedFace (      WingedFace&&) = default;

    const Id&              id              () const { return this->_id.id (); }
    WingedEdge*            edge            () const { return this->_edge; }
    OctreeNode*            octreeNode      () const { return this->_octreeNode; }
    unsigned int           index           () const { return this->_index; }

    void                   edge            (WingedEdge*  e) { this->_edge       = e; }
    void                   octreeNode      (OctreeNode*  n) { this->_octreeNode = n; }
    void                   index           (unsigned int i) { this->_index      = i; }
    void                   writeIndices    (WingedMesh&);
    PrimTriangle           triangle        (const WingedMesh&) const;
    WingedVertex&          firstVertex     () const;
    WingedVertex&          secondVertex    () const;
    WingedVertex&          thirdVertex     () const;
    unsigned int           numEdges        () const;
    glm::vec3              normal          (const WingedMesh&) const;

    /** `adjacent (v,b)` returns an adjacent edge with `v` as start or end point.
     * It's crucial that `v` is actually a vertex of `this` face.
     */
    WingedEdge*            adjacent               (const WingedVertex&) const;
    WingedEdge*            longestEdge            (const WingedMesh&, float*) const;
    float                  longestEdgeLengthSqr   (const WingedMesh&) const;
    bool                   isTriangle             () const;

    AdjEdges               adjacentEdges          (WingedEdge&) const;
    AdjEdges               adjacentEdges          ()            const;
    AdjVertices            adjacentVertices       (WingedEdge&) const;
    AdjVertices            adjacentVertices       ()            const;
    AdjFaces               adjacentFaces          (WingedEdge&) const;
    AdjFaces               adjacentFaces          ()            const;

    SAFE_REF_CONST  (WingedEdge, edge)
    SAFE_REF_CONST  (OctreeNode, octreeNode)
    SAFE_REF1_CONST (WingedEdge, adjacent, const WingedVertex&)
    SAFE_REF2_CONST (WingedEdge, longestEdge, const WingedMesh&, float*)
  private:
    const IdObject  _id;
    WingedEdge*     _edge;
    OctreeNode*     _octreeNode;
    unsigned int    _index;
};

#endif

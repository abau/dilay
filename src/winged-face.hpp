#ifndef DILAY_WINGED_FACE
#define DILAY_WINGED_FACE

#include <vector>
#include <unordered_set>
#include "id.hpp"
#include "fwd-winged.hpp"
#include "fwd-glm.hpp"

class AdjacentEdgeIterator;
class AdjacentVertexIterator;
class AdjacentFaceIterator;
class Triangle;
template <class T> class Maybe;

class WingedFace {
  public:                    WingedFace    ();
                             WingedFace    (LinkedEdge);

    IdType                   id            () const { return this->_id.get (); }
    LinkedEdge               edge          () const { return this->_edge; }

    void                     setEdge       (LinkedEdge);
    void                     addIndices    (WingedMesh&);
    void                     triangle      (const WingedMesh&, Triangle&) const;
    Triangle                 triangle      (const WingedMesh&) const;
    glm::vec3                normal        (const WingedMesh&) const;
    unsigned int             numEdges      () const;

    /** `adjacent (v)` returns an adjacent edge with `v` as start or end point.
     * It's crucial that `v` is acutally a vertex of `this` face
     */
    LinkedEdge               adjacent      (const WingedVertex&) const;
    LinkedEdge               longestEdge   (const WingedMesh&) const;
    Maybe<LinkedVertex>      tVertex       () const;
    Maybe<LinkedEdge>        tEdge         () const;
    unsigned int             level         () const;

    LinkedVertex             highestLevelVertex     () const;
    AdjacentEdgeIterator     adjacentEdgeIterator   (bool = false) const;
    AdjacentVertexIterator   adjacentVertexIterator (bool = false) const;
    AdjacentFaceIterator     adjacentFaceIterator   (bool = false) const;
    AdjacentEdgeIterator     adjacentEdgeIterator   (LinkedEdge, bool = false) const;
    AdjacentVertexIterator   adjacentVertexIterator (LinkedEdge, bool = false) const;
    AdjacentFaceIterator     adjacentFaceIterator   (LinkedEdge, bool = false) const;

  private:
    const Id     _id;
    LinkedEdge   _edge;
};

#endif

#ifndef DILAY_WINGED_VERTEX
#define DILAY_WINGED_VERTEX

#include <glm/fwd.hpp>

class AdjacentEdgeIterator;
class AdjacentVertexIterator;
class AdjacentFaceIterator;
class WingedEdge;
class WingedMesh;

class WingedVertex {
  public: 
    WingedVertex (unsigned int, WingedEdge*);
    WingedVertex (const WingedVertex&)  = default;
    WingedVertex (      WingedVertex&&) = default;

    unsigned int index       () const { return this->_index; }
    WingedEdge*  edge        () const { return this->_edge;  }

    void         edge        (WingedEdge*);
    unsigned int writeIndex  (WingedMesh&);
    void         writeIndex  (WingedMesh&, unsigned int);
    glm::vec3    vector      (const WingedMesh&) const;
    glm::vec3    normal      (const WingedMesh&) const;
    void         writeNormal (WingedMesh&) const;
    unsigned int valence     () const;
    WingedEdge*  tEdge       () const;

    AdjacentEdgeIterator    adjacentEdgeIterator   (bool = false) const;
    AdjacentVertexIterator  adjacentVertexIterator (bool = false) const;
    AdjacentFaceIterator    adjacentFaceIterator   () const;
    AdjacentEdgeIterator    adjacentEdgeIterator   (WingedEdge&, bool = false) const;
    AdjacentVertexIterator  adjacentVertexIterator (WingedEdge&, bool = false) const;
    AdjacentFaceIterator    adjacentFaceIterator   (WingedEdge&) const;

  private:
    const unsigned int          _index;
    WingedEdge*                 _edge;
};

#endif

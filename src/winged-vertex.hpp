#ifndef DILAY_WINGED_VERTEX
#define DILAY_WINGED_VERTEX

#include "fwd-glm.hpp"

template <class T> class Maybe;
class AdjacentEdgeIterator;
class AdjacentVertexIterator;
class AdjacentFaceIterator;
class WingedEdge;
class WingedMesh;

class WingedVertex {
  public: WingedVertex     (unsigned int, WingedEdge*, unsigned int);

    unsigned int index     () const { return this->_index; }
    WingedEdge*  edge      () const { return this->_edge;  }
    unsigned int level     () const { return this->_level; }

    void         edge      (WingedEdge*);
    void         addIndex  (WingedMesh&);
    glm::vec3    vertex    (const WingedMesh&) const;

    glm::vec3    normal    (const WingedMesh&) const;
    unsigned int valence   () const;
    WingedEdge*  tEdge     () const;

    AdjacentEdgeIterator    adjacentEdgeIterator   (bool = false) const;
    AdjacentVertexIterator  adjacentVertexIterator (bool = false) const;
    AdjacentFaceIterator    adjacentFaceIterator   () const;
    AdjacentEdgeIterator    adjacentEdgeIterator   (WingedEdge&, bool = false) const;
    AdjacentVertexIterator  adjacentVertexIterator (WingedEdge&, bool = false) const;
    AdjacentFaceIterator    adjacentFaceIterator   (WingedEdge&) const;

  private:
    const unsigned int          _index;
    WingedEdge*                 _edge;
    const unsigned int          _level;
    bool                        _isTVertex;
    bool                        _isOVertex;
};

#endif

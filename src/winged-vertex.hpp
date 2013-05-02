#ifndef DILAY_WINGED_VERTEX
#define DILAY_WINGED_VERTEX

#include <vector>
#include "fwd-glm.hpp"
#include "fwd-winged.hpp"

template <class T> class Maybe;
class AdjacentEdgeIterator;
class AdjacentVertexIterator;
class AdjacentFaceIterator;

class WingedVertex {
  public:                      
    WingedVertex (unsigned int, unsigned int);
    WingedVertex (unsigned int, LinkedEdge, unsigned int);

    unsigned int       index     () const { return this->_index; }
    LinkedEdge         edge      () const { return this->_edge;  }
    unsigned int       level     () const { return this->_level; }

    void               setEdge   (LinkedEdge);
    void               addIndex  (WingedMesh&);
    glm::vec3          vertex    (const WingedMesh&) const;

    glm::vec3          normal    (const WingedMesh&) const;
    unsigned int       valence   () const;
    Maybe <LinkedEdge> tEdge     () const;

    AdjacentEdgeIterator    adjacentEdgeIterator   (bool = false) const;
    AdjacentVertexIterator  adjacentVertexIterator (bool = false) const;
    AdjacentFaceIterator    adjacentFaceIterator   () const;
    AdjacentEdgeIterator    adjacentEdgeIterator   (LinkedEdge, bool = false) const;
    AdjacentVertexIterator  adjacentVertexIterator (LinkedEdge, bool = false) const;
    AdjacentFaceIterator    adjacentFaceIterator   (LinkedEdge) const;

  private:
    const unsigned int          _index;
    LinkedEdge                  _edge;
    const unsigned int          _level;
    bool                        _isTVertex;
    bool                        _isOVertex;
};

#endif

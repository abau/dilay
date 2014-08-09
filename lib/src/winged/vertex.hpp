#ifndef DILAY_WINGED_VERTEX
#define DILAY_WINGED_VERTEX

#include <glm/fwd.hpp>
#include "macro.hpp"

class AdjEdges;
class AdjVertices;
class AdjFaces;
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

    AdjEdges     adjacentEdges    (WingedEdge&, bool = false) const;
    AdjEdges     adjacentEdges    (bool = false)              const;
    AdjVertices  adjacentVertices (WingedEdge&, bool = false) const;
    AdjVertices  adjacentVertices (bool = false)              const;
    AdjFaces     adjacentFaces    (WingedEdge&)               const;
    AdjFaces     adjacentFaces    ()                          const;

    SAFE_REF_CONST (WingedEdge, edge)
  private:
    const unsigned int          _index;
    WingedEdge*                 _edge;
};

#endif

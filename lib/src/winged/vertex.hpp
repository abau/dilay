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
    WingedVertex (unsigned int);
    WingedVertex (const WingedVertex&)  = default;
    WingedVertex (      WingedVertex&&) = default;

    unsigned int index              () const { return this->_index; }
    WingedEdge*  edge               () const { return this->_edge;  }

    void         edge               (WingedEdge*);
    void         writeIndex         (WingedMesh&, unsigned int);
    glm::vec3    vector             (const WingedMesh&) const;
    glm::vec3    savedNormal        (const WingedMesh&) const;
    glm::vec3    interpolatedNormal (const WingedMesh&) const;
    void         writePosition      (WingedMesh&, const glm::vec3&);
    void         writeNormal        (WingedMesh&, const glm::vec3&);
    unsigned int valence            () const;

    AdjEdges     adjacentEdges    (WingedEdge&) const;
    AdjEdges     adjacentEdges    ()            const;
    AdjVertices  adjacentVertices (WingedEdge&) const;
    AdjVertices  adjacentVertices ()            const;
    AdjFaces     adjacentFaces    (WingedEdge&) const;
    AdjFaces     adjacentFaces    ()            const;

    SAFE_REF_CONST (WingedEdge, edge)
  private:
    const unsigned int _index;
    WingedEdge*        _edge;
};

#endif

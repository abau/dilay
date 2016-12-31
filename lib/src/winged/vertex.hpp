/* This file is part of Dilay
 * Copyright © 2015,2016 Alexander Bau
 * Use and redistribute under the terms of the GNU General Public License
 */
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

    bool             operator== (const WingedVertex&) const;
    bool             operator!= (const WingedVertex&) const;

    unsigned int     index    () const { return this->_index; }
    WingedEdge*      edge     () const { return this->_edge; }

    void             edge     (WingedEdge* e) { this->_edge = e; }

    void             writeIndex            (WingedMesh&, unsigned int);
    const glm::vec3& position              (const WingedMesh&) const;
    const glm::vec3& savedNormal           (const WingedMesh&) const;
    glm::vec3        interpolatedNormal    (const WingedMesh&) const;
    bool             isNewVertex           (const WingedMesh&) const;
    void             position              (WingedMesh&, const glm::vec3&);
    void             normal                (WingedMesh&, const glm::vec3&);
    void             setInterpolatedNormal (WingedMesh&);
    void             isNewVertex           (WingedMesh&, bool);
    unsigned int     valence               () const;
    AdjEdges         adjacentEdges         () const;
    AdjVertices      adjacentVertices      () const;
    AdjFaces         adjacentFaces         () const;

    SAFE_REF_CONST (WingedEdge, edge)
  private:
    unsigned int _index;
    WingedEdge*  _edge;
};

#endif

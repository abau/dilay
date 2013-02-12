#include <glm/glm.hpp>
#include <set>
#include "linked-list.hpp"

#ifndef WINGED_VERTEX
#define WINGED_VERTEX

class WingedMesh;
class WingedFace;
class WingedEdge;

class WingedVertex {
  public:
    WingedVertex (unsigned int, LinkedElement <WingedEdge>*);

    unsigned int           index         () const { return this->_index; }
    const LinkedElement <WingedEdge>* edge () const { return this->_edge;}

    void                   setEdge       (LinkedElement <WingedEdge>*);
    void                   addIndex      (WingedMesh&);
    glm::vec3              vertex        (const WingedMesh&) const;
    std::set <WingedEdge*> adjacentEdges () const;
    std::set <WingedFace*> adjacentFaces () const;
    glm::vec3              normal        (const WingedMesh&) const;

  private:
    const unsigned int           _index;
    LinkedElement <WingedEdge>*  _edge;
};

#endif

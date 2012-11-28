#include <glm/glm.hpp>
#include "linked-list.hpp"

#ifndef WINGED_VERTEX
#define WINGED_VERTEX

class WingedMesh;
class WingedEdge;

class WingedVertex {
  public:
    WingedVertex (unsigned int, LinkedElement <WingedEdge>*);

    unsigned int index    () const { return this->_index; }

    void         setEdge  (LinkedElement <WingedEdge>*);
    void         addIndex (WingedMesh&);
    glm::vec3    vertex   (const WingedMesh&) const;

  private:
    const unsigned int           _index;
    LinkedElement <WingedEdge>*  edge;
};

#endif

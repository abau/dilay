#include <glm/glm.hpp>
#include <vector>
#include "linked-list.hpp"

#ifndef WINGED_VERTEX
#define WINGED_VERTEX

class WingedMesh;
class WingedFace;
class WingedEdge;

class WingedVertex {
  public:
    WingedVertex (unsigned int, LinkedElement <WingedEdge>*);

    unsigned int                index () const { return this->_index; }
    LinkedElement <WingedEdge>* edge  () const { return this->_edge;}

    void                        setEdge           (LinkedElement <WingedEdge>*);
    void                        addIndex          (WingedMesh&);
    glm::vec3                   vertex            (const WingedMesh&) const;

    std::vector <WingedFace*>   adjacentFaces     () const;
    /**
     * `adjacentEdges (e)` returns a vector of adjacent edges in counterclockwise
     * order starting at `e`. It's crucial that `e` is an actual adjacent edge.
     */
    std::vector <WingedEdge*>   adjacentEdges     (WingedEdge&) const;
    std::vector <WingedEdge*>   adjacentEdges     () const;
    /**
     * `adjacentVertices (e)` returns a vector of adjacent vertices in counterclockwise
     * order starting with the vertex along edge `e`. 
     * It's crucial that `e` is an actual adjacent edge.
     */
    std::vector <WingedVertex*> adjacentVertices  (WingedEdge&) const;
    std::vector <WingedVertex*> adjacentVertices  () const;
    glm::vec3                   normal            (const WingedMesh&) const;

  private:
    const unsigned int           _index;
    LinkedElement <WingedEdge>*  _edge;
};

#endif

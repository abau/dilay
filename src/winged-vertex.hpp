#ifndef WINGED_VERTEX
#define WINGED_VERTEX

#include <glm/glm.hpp>
#include <vector>
#include "util.hpp"

class WingedVertex {
  public:                      
    WingedVertex (unsigned int, LinkedEdge);

    unsigned int                index () const { return this->_index; }
    LinkedEdge                  edge  () const { return this->_edge;}

    void                        setEdge           (LinkedEdge);
    void                        addIndex          (WingedMesh&);
    glm::vec3                   vertex            (const WingedMesh&) const;

    /**
     * `adjacentEdges (e)` returns a vector of adjacent edges in counterclockwise
     * order starting at `e`. It's crucial that `e` is an actual adjacent edge.
     */
    std::vector <LinkedEdge>    adjacentEdges     (LinkedEdge) const;
    std::vector <LinkedEdge>    adjacentEdges     () const;
    std::vector <LinkedFace>    adjacentFaces     () const;
    /**
     * `adjacentVertices (e)` returns a vector of adjacent vertices in counterclockwise
     * order starting with the vertex along edge `e`. 
     * It's crucial that `e` is an actual adjacent edge.
     */
    std::vector <LinkedVertex>  adjacentVertices  (LinkedEdge) const;
    std::vector <LinkedVertex>  adjacentVertices  () const;
    glm::vec3                   normal            (const WingedMesh&) const;
    unsigned int                valence           () const;

  private:
    const unsigned int          _index;
    LinkedEdge                  _edge;
};

#endif

#ifndef WINGED_FACE
#define WINGED_FACE

#include "id.hpp"
#include "util.hpp"
#include "triangle.hpp"
#include "winged-edge-iterator.hpp"

#define WINGED_EDGE_ITERATOR(it,face) WingedEdgeIterator it = (face).edgeIterator (); it.hasEdge (); it.next ()

class WingedFace {
  public:              WingedFace    (LinkedEdge);

    IdType             id            () const { return this->_id.get (); }
    LinkedEdge         edge          () const { return this->_edge; }

    void               setEdge       (LinkedEdge);
    void               addIndices    (WingedMesh&);
    Triangle           triangle      (const WingedMesh&) const;
    glm::vec3          normal        (const WingedMesh&) const;
    WingedEdgeIterator edgeIterator  () const;
    unsigned int       numEdges      () const;

  private:
    const Id   _id;
    LinkedEdge _edge;
};

#endif

#include "id.hpp"
#include "util.hpp"
#include "triangle.hpp"

#ifndef WINGED_FACE
#define WINGED_FACE

class WingedMesh;
class WingedEdge;

class WingedFace {
  public:
    WingedFace (LinkedEdge*);

    IdType      id            () const { return this->_id.get (); }
    LinkedEdge* edge          ()       { return this->_edge; }

    void        setEdge       (LinkedElement <WingedEdge>*);
    void        addIndices    (WingedMesh&);
    Triangle    triangle      (const WingedMesh&) const;
    LinkedEdge& longestEdge   (const WingedMesh&);

  private:
    const Id    _id;
    LinkedEdge* _edge;
};

#endif

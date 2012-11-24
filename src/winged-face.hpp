#include "id.hpp"
#include "util.hpp"
#include "triangle.hpp"
#include "depth.hpp"

#ifndef WINGED_FACE
#define WINGED_FACE

class WingedMesh;
class WingedEdge;

class WingedFace {
  private:
    const Id    _id;
    LinkedEdge* _edge;
    Depth       _depth;

  public:
    WingedFace (LinkedEdge*, Depth);

    IdType      id            () const { return this->_id.get (); }
    LinkedEdge* edge          ()       { return this->_edge; }
    Depth       depth         () const { return this->_depth; }

    void        setEdge       (LinkedElement <WingedEdge>*);
    void        addIndices    (WingedMesh&);
    Triangle    triangle      (const WingedMesh&) const;
    LinkedEdge& longestEdge   (const WingedMesh&);
    void        increaseDepth ();
    void        decreaseDepth ();
};

#endif

#ifndef DILAY_PARITAL_ACTION_INSERT_VERTEX
#define DILAY_PARITAL_ACTION_INSERT_VERTEX

#include "action/on-winged-mesh.hpp"
#include "fwd-glm.hpp"
#include "macro.hpp"

class WingedEdge;
class WingedMesh;

class PAInsertEdgeVertex : public ActionOnWMesh {
  public: 
    DECLARE_ACTION_BIG5 (PAInsertEdgeVertex)

    /** `run (m,e,v,g)` inserts a new vertex `v` at edge `e`.
     * The new edge `n` (from the first vertex of `e` to `v`) is returned.
     * If `g == true` the edge gradients of `e` and `n` are set.
     */
    WingedEdge& run (WingedMesh&, WingedEdge&, const glm::vec3&, bool = false);

    void undo (WingedMesh&);
    void redo (WingedMesh&);
  private:
    class Impl;
    Impl* impl;
};
#endif

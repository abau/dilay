#ifndef DILAY_PARITAL_ACTION_INSERT_VERTEX
#define DILAY_PARITAL_ACTION_INSERT_VERTEX

#include <glm/fwd.hpp>
#include "action/on.hpp"
#include "macro.hpp"

class WingedEdge;
class WingedMesh;

class PAInsertEdgeVertex : public ActionOn <WingedMesh> {
  public: 
    DECLARE_BIG3 (PAInsertEdgeVertex)

    /** `run (m,e,v)` inserts a new vertex `v` at edge `e`.
     * The new edge `n` (from the first vertex of `e` to `v`) is returned.
     */
    WingedEdge& run (WingedMesh&, WingedEdge&, const glm::vec3&);

  private:
    void runUndo (WingedMesh&);
    void runRedo (WingedMesh&);

    class Impl;
    Impl* impl;
};
#endif

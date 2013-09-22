#ifndef DILAY_ACTION_INSERT_VERTEX
#define DILAY_ACTION_INSERT_VERTEX

#include "action.hpp"
#include "fwd-glm.hpp"

class WingedEdge;
class WingedMesh;

class ActionInsertVertex : public Action {
  public: ActionInsertVertex            ();
          ActionInsertVertex            (const ActionInsertVertex&) = delete;
    const ActionInsertVertex& operator= (const ActionInsertVertex&) = delete;
         ~ActionInsertVertex            ();

    /** `run (m,e,v)` inserts a new vertex `v` at edge `e`.
     * The new edge (from the first vertex of `e` to `v`) is returned.
     */
    WingedEdge& run (WingedMesh&, WingedEdge&, const glm::vec3&);

    void undo ();
    void redo ();
  private:
    class Impl;
    Impl* impl;
};
#endif

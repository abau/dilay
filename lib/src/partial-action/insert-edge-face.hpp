#ifndef DILAY_PARTIAL_ACTION_INSERT_EDGE_FACE
#define DILAY_PARTIAL_ACTION_INSERT_EDGE_FACE

#include <glm/fwd.hpp>
#include "action/on.hpp"
#include "macro.hpp"

class WingedMesh;
class WingedEdge;
class WingedFace;

class PAInsertEdgeFace : public ActionOn <WingedMesh> {
  public: 
    DECLARE_BIG3 (PAInsertEdgeFace)

    /** `run (m,f)` inserts a new edge `e` from `f.edge ()->secondVertex (f)`
     * and a new triangle on the left-hand side of `e`.
     */
    WingedEdge& run (WingedMesh&, WingedFace&);

  private:
    void runUndo (WingedMesh&) const;
    void runRedo (WingedMesh&) const;

    IMPLEMENTATION
};
#endif

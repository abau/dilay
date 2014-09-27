#ifndef DILAY_PARTIAL_ACTION_TRIANGULATE_QUAD
#define DILAY_PARTIAL_ACTION_TRIANGULATE_QUAD

#include <unordered_set>
#include "action/on.hpp"
#include "macro.hpp"

class WingedMesh;
class WingedFace;

class PATriangulateQuad : public ActionOn <WingedMesh> {
  public: 
    DECLARE_BIG3 (PATriangulateQuad)

    /* `run (m,f,n)` triangulates the quad `f` by inserting an edge from 
     * `f.edge ()->secondVertex (f)`.
     * `f` and the new face are added to `n` if `n` is not `nullptr`.
     */
    WingedEdge& run (WingedMesh&, WingedFace&, std::unordered_set <WingedFace*>*);

  private:
    void runUndo (WingedMesh&);
    void runRedo (WingedMesh&);

    class Impl;
    Impl* impl;
};
#endif

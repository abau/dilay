#ifndef DILAY_PARTIAL_ACTION_DELETE_EDGE_FACE
#define DILAY_PARTIAL_ACTION_DELETE_EDGE_FACE

#include "action.hpp"
#include "fwd-glm.hpp"

class WingedEdge;
class WingedMesh;

class PADeleteEdgeFace : public Action {
  public: PADeleteEdgeFace            ();
          PADeleteEdgeFace            (const PADeleteEdgeFace&) = delete;
    const PADeleteEdgeFace& operator= (const PADeleteEdgeFace&) = delete;
         ~PADeleteEdgeFace            ();

    /** `run (m,e)` deletes edge `e` and its _right_ face of mesh `m`. 
     * Note that other parts of the program depend on this behaviour. */
    void run (WingedMesh&, WingedEdge&);

    void undo ();
    void redo ();
  private:
    class Impl;
    Impl* impl;
};
#endif

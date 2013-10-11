#ifndef DILAY_PARTIAL_ACTION_TRIANGULATE_QUAD
#define DILAY_PARTIAL_ACTION_TRIANGULATE_QUAD

#include "action.hpp"

class WingedFace;
class WingedMesh;

class PATriangulateQuad : public Action {
  public: PATriangulateQuad            ();
          PATriangulateQuad            (const PATriangulateQuad&) = delete;
    const PATriangulateQuad& operator= (const PATriangulateQuad&) = delete;
         ~PATriangulateQuad            ();

    /** `run (m,f)` triangulates the quad `f` by 
     * inserting an edge from the vertex of `f` with the heighest level. */
    void run (WingedMesh&, WingedFace&);

    void undo ();
    void redo ();
  private:
    class Impl;
    Impl* impl;
};
#endif

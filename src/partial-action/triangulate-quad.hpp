#ifndef DILAY_PARTIAL_ACTION_TRIANGULATE_QUAD
#define DILAY_PARTIAL_ACTION_TRIANGULATE_QUAD

#include <list>
#include "action.hpp"

class WingedFace;
class WingedMesh;
class Id;

class PATriangulateQuad : public Action {
  public: PATriangulateQuad            ();
          PATriangulateQuad            (const PATriangulateQuad&) = delete;
    const PATriangulateQuad& operator= (const PATriangulateQuad&) = delete;
         ~PATriangulateQuad            ();

    /** `run (m,f,a)` triangulates the quad `f` by 
     * inserting an edge from the vertex of `f` with the heighest level. 
     * `f` and the new face are inserted into `a`, if `a` is not `nullptr`.*/
    void run (WingedMesh&, WingedFace&, std::list <Id>* = nullptr);

    void undo ();
    void redo ();
  private:
    class Impl;
    Impl* impl;
};
#endif

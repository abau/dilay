#ifndef DILAY_PARTIAL_ACTION_TRIANGULATE_QUAD
#define DILAY_PARTIAL_ACTION_TRIANGULATE_QUAD

#include <list>
#include "action/on-winged-mesh.hpp"
#include "macro.hpp"

class WingedFace;
class WingedMesh;
class Id;

class PATriangulateQuad : public ActionOnWMesh {
  public: 
    DECLARE_ACTION_BIG5 (PATriangulateQuad)

    /** `run (m,f,a)` triangulates the quad `f` by 
     * inserting an edge from the vertex of `f` with the heighest level. 
     * `f` and the new face are inserted into `a`, if `a` is not `nullptr`.*/
    void run (WingedMesh&, WingedFace&, std::list <Id>* = nullptr);

    void undo (WingedMesh&);
    void redo (WingedMesh&);
  private:
    class Impl;
    Impl* impl;
};
#endif

#ifndef DILAY_PARTIAL_ACTION_MODIFY_WINGED_FACE
#define DILAY_PARTIAL_ACTION_MODIFY_WINGED_FACE

#include "action/on-winged-mesh.hpp"
#include "macro.hpp"

class WingedMesh;
class WingedEdge;
class WingedFace;

class PAModifyWFace : public ActionOnWMesh {
  public: 
    DECLARE_ACTION_BIG6 (PAModifyWFace)

    void edge  (WingedFace&, WingedEdge*);
    void write (WingedMesh&, WingedFace&);

  private:
    void runUndo (WingedMesh&);
    void runRedo (WingedMesh&);

    class Impl;
    Impl* impl;
};
#endif

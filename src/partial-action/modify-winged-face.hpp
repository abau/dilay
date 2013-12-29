#ifndef DILAY_PARTIAL_ACTION_MODIFY_WINGED_FACE
#define DILAY_PARTIAL_ACTION_MODIFY_WINGED_FACE

#include "action.hpp"
#include "macro.hpp"

class WingedMesh;
class WingedEdge;
class WingedFace;

class PAModifyWFace : public Action {
  public: 
    DECLARE_ACTION_BIG5 (PAModifyWFace)

    void edge  (WingedMesh&, WingedFace&, WingedEdge*);
    void write (WingedMesh&, WingedFace&);

    void undo ();
    void redo ();
  private:
    class Impl;
    Impl* impl;
};
#endif

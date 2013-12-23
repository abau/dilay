#ifndef DILAY_PARTIAL_ACTION_MODIFY_WINGED_FACE
#define DILAY_PARTIAL_ACTION_MODIFY_WINGED_FACE

#include "action.hpp"

class WingedMesh;
class WingedEdge;
class WingedFace;

class PAModifyWFace : public Action {
  public: PAModifyWFace            ();
          PAModifyWFace            (const PAModifyWFace&) = delete;
    const PAModifyWFace& operator= (const PAModifyWFace&) = delete;
         ~PAModifyWFace            ();

    void edge  (WingedMesh&, WingedFace&, WingedEdge*);
    void write (WingedMesh&, WingedFace&);

    void undo ();
    void redo ();
  private:
    class Impl;
    Impl* impl;
};
#endif

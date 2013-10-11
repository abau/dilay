#ifndef DILAY_PARTIAL_ACTION_MODIFY_FACE
#define DILAY_PARTIAL_ACTION_MODIFY_FACE

#include "action.hpp"

class WingedMesh;
class WingedEdge;
class WingedFace;

class PAModifyFace : public Action {
  public: PAModifyFace            ();
          PAModifyFace            (const PAModifyFace&) = delete;
    const PAModifyFace& operator= (const PAModifyFace&) = delete;
         ~PAModifyFace            ();

    void edge  (WingedMesh&, WingedFace&, WingedEdge*);
    void write (WingedMesh&, WingedFace&);

    void undo ();
    void redo ();
  private:
    class Impl;
    Impl* impl;
};
#endif

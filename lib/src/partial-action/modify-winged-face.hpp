#ifndef DILAY_PARTIAL_ACTION_MODIFY_WINGED_FACE
#define DILAY_PARTIAL_ACTION_MODIFY_WINGED_FACE

#include "action/on.hpp"
#include "macro.hpp"

class WingedMesh;
class WingedEdge;
class WingedFace;

class PAModifyWFace : public ActionOn <WingedMesh> {
  public: 
    DECLARE_BIG3 (PAModifyWFace)

    void edge         (WingedFace&, WingedEdge*);
    void reset        (WingedFace&);
    void writeIndices (WingedMesh&, WingedFace&);

  private:
    void runUndo (WingedMesh&) const;
    void runRedo (WingedMesh&) const;

    IMPLEMENTATION
};
#endif

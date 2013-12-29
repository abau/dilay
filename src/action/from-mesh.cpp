#include "action/from-mesh.hpp"

struct ActionFromMesh :: Impl {
  ActionUnit actions;

  void undo () { this->actions.undo (); }
  void redo () { this->actions.redo (); }

  void run (WingedMesh& w, const Mesh& mesh) {

  }
};

DELEGATE_ACTION_BIG5 (ActionFromMesh)
DELEGATE2            (void, ActionFromMesh, cube, WingedMesh&, const Mesh&);
DELEGATE             (void, ActionFromMesh, undo)
DELEGATE             (void, ActionFromMesh, redo)

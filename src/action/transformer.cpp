#include <memory>
#include "action/transformer.hpp"
#include "action/ids.hpp"
#include "action/on-winged-mesh.hpp"

struct ActionTransformer :: Impl {
  ActionIds                       operands;
  std::unique_ptr <ActionOnWMesh> onWMesh;

  Impl (WingedMesh& mesh, ActionOnWMesh* action) {
    this->operands.setMesh (0,&mesh);
    this->onWMesh.reset    (action);
  }

  void runUndo () {
    if (this->onWMesh) {
      this->onWMesh->undo (this->operands.getMesh (0));
    }
  }

  void runRedo () {
    if (this->onWMesh) {
      this->onWMesh->redo (this->operands.getMesh (0));
    }
  }
};

DELEGATE2_BIG3 (ActionTransformer,WingedMesh&,ActionOnWMesh*)

DELEGATE (void, ActionTransformer, runUndo)
DELEGATE (void, ActionTransformer, runRedo)

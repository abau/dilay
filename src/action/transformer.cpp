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

  void undo () {
    if (this->onWMesh) {
      this->onWMesh->undo (this->operands.getMesh (0));
    }
  }

  void redo () {
    if (this->onWMesh) {
      this->onWMesh->redo (this->operands.getMesh (0));
    }
  }
};

DELEGATE2_CONSTRUCTOR     (ActionTransformer,WingedMesh&,ActionOnWMesh*)
DELEGATE_MOVE_CONSTRUCTOR (ActionTransformer)
DELEGATE_DESTRUCTOR       (ActionTransformer)

DELEGATE (void, ActionTransformer, undo)
DELEGATE (void, ActionTransformer, redo)

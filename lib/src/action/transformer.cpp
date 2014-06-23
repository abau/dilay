#include <memory>
#include "action/transformer.hpp"
#include "action/ids.hpp"
#include "action/on.hpp"

struct ActionTransformer :: Impl {
  ActionIds                               operands;
  std::unique_ptr <ActionOn <WingedMesh>> onWMesh;

  Impl (WingedMesh& mesh, ActionOn <WingedMesh>& action) {
    this->operands.setMesh (0,mesh);
    this->onWMesh.reset    (&action);
  }

  void runUndo () {
    this->onWMesh->undo (this->operands.getWingedMesh (0));
  }

  void runRedo () {
    this->onWMesh->redo (this->operands.getWingedMesh (0));
  }
};

DELEGATE2_BIG3 (ActionTransformer,WingedMesh&,ActionOn <WingedMesh>&)

DELEGATE (void, ActionTransformer, runUndo)
DELEGATE (void, ActionTransformer, runRedo)

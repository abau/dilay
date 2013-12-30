#include "action/unit/on-winged-mesh.hpp"
#include "action/unit/template.hpp"

struct ActionUnitOnWMesh :: Impl {
  ActionUnitTemplate <ActionOnWMesh> unitTemplate;

  void undo (WingedMesh& mesh) {
    this->unitTemplate.forallReverse ([&mesh] (ActionOnWMesh& a) { a.undo (mesh); });
  }

  void redo (WingedMesh& mesh) {
    this->unitTemplate.forall ([&mesh] (ActionOnWMesh& a) { a.redo (mesh); });
  }
};

DELEGATE_ACTION_BIG6      (ActionUnitOnWMesh)
DELEGATE_TO_UNIT_TEMPLATE (ActionUnitOnWMesh,ActionOnWMesh)

DELEGATE1 (void, ActionUnitOnWMesh, undo, WingedMesh&)
DELEGATE1 (void, ActionUnitOnWMesh, redo, WingedMesh&)

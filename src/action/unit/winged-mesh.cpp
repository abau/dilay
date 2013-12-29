#include "action/unit/winged-mesh.hpp"
#include "action/unit/template.hpp"

struct ActionUnitWMesh :: Impl {
  ActionUnitTemplate <ActionOnWMesh> unitTemplate;

  void undo (WingedMesh& mesh) {
    this->unitTemplate.forallReverse ([&mesh] (ActionOnWMesh& a) { a.undo (mesh); });
  }

  void redo (WingedMesh& mesh) {
    this->unitTemplate.forall ([&mesh] (ActionOnWMesh& a) { a.redo (mesh); });
  }
};

DELEGATE_ACTION_BIG5      (ActionUnitWMesh)
DELEGATE_TO_UNIT_TEMPLATE (ActionUnitWMesh,ActionOnWMesh)

DELEGATE1 (void, ActionUnitWMesh, undo, WingedMesh&)
DELEGATE1 (void, ActionUnitWMesh, redo, WingedMesh&)

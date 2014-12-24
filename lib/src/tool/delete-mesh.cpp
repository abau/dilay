#include <QObject>
#include "action/delete-winged-mesh.hpp"
#include "action/unit.hpp"
#include "history.hpp"
#include "scene.hpp"
#include "selection.hpp"
#include "selection-mode.hpp"
#include "state.hpp"
#include "tools.hpp"

struct ToolDeleteMesh::Impl {
  ToolDeleteMesh* self;

  Impl (ToolDeleteMesh* s) : self (s) {}

  ToolResponse runInitialize () {
    ActionUnit unit;

    switch (this->self->state ().scene ().selectionMode ()) {
      case SelectionMode::WingedMesh:
        this->runDeleteWingedMesh (unit);
        break;
    }

    if (unit.isEmpty () == false) {
      this->self->state ().history ().addUnit     (std::move (unit));
      this->self->state ().scene   ().unselectAll ();
    }
    return ToolResponse::Terminate;
  }

  void runDeleteWingedMesh (ActionUnit& unit) {
    this->self->state ().scene ().selection ().forEachMajor ([this,&unit]
      (unsigned int index) {
        WingedMesh& mesh = this->self->state ().scene ().wingedMeshRef (index);
        unit.add <ActionDeleteWMesh> ().run (this->self->state ().scene (), mesh);
      }
    );
  }
};

DELEGATE_TOOL                (ToolDeleteMesh)
DELEGATE_TOOL_RUN_INITIALIZE (ToolDeleteMesh)

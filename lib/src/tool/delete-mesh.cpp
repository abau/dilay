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

  static QString toolName () {
    return QObject::tr ("Delete Mesh");
  }

  ToolResponse runInitialize () {
    ActionUnit unit;

    switch (State::scene ().selectionMode ()) {
      case SelectionMode::WingedMesh:
        this->runDeleteWingedMesh (unit);
        break;
    }

    if (unit.isEmpty () == false) {
      State::history ().addUnit     (std::move (unit));
      State::scene   ().unselectAll ();
    }
    return ToolResponse::Terminate;
  }

  void runDeleteWingedMesh (ActionUnit& unit) {
    State::scene ().selection ().forEachMajor ([this,&unit]
      (unsigned int index) {
        WingedMesh& mesh = State::scene ().wingedMeshRef (index);
        unit.add <ActionDeleteWMesh> ().run (mesh);
      }
    );
  }
};

DELEGATE_TOOL                (ToolDeleteMesh)
DELEGATE_TOOL_RUN_INITIALIZE (ToolDeleteMesh)

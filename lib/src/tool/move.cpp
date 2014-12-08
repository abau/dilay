#include <QObject>
#include <glm/glm.hpp>
#include <list>
#include "action/modify-winged-mesh.hpp"
#include "action/unit.hpp"
#include "fwd-winged.hpp"
#include "history.hpp"
#include "mesh-type.hpp"
#include "scene.hpp"
#include "selection-mode.hpp"
#include "selection.hpp"
#include "state.hpp"
#include "tool/util/movement.hpp"
#include "tools.hpp"
#include "variant.hpp"
#include "view/gl-widget.hpp"
#include "view/main-window.hpp"
#include "view/tool/menu-parameters.hpp"
#include "winged/mesh.hpp"

namespace {
  typedef Variant <MeshPtrList> Entities;
};

struct ToolMove::Impl {
  ToolMove*        self;
  Entities         entities;
  ToolUtilMovement movement;

  Impl (ToolMove* s, MovementConstraint c) 
    : self     (s) 
    , entities (std::move (Impl::getEntities ()))
    , movement (c, s->menuParameters ().mainWindow ().glWidget ().cursorPosition ())
  {
  }

  static QString toolName (MovementConstraint) {
    return QObject::tr ("Move");
  }

  static Entities getEntities () {
          Entities   entities;
          Scene&     scene     = State::scene ();
    const Selection& selection = scene.selection ();

    switch (scene.selectionMode ()) {
      case SelectionMode::Freeform: {
        MeshPtrList meshes;
        scene.forEachSelectedMesh ([&meshes] (WingedMesh& m) { meshes.push_back (&m); });
        entities.set <MeshPtrList> (meshes);
        break;
      }
      default:
        std::abort ();
    }
    return entities;
  }

  void translateSelectionBy (const glm::vec3& t) {
    this->entities.caseOf <void>
      ( [&t] (MeshPtrList& ms) { for (WingedMesh* m : ms) { m->translate (t); } }
      );
  }

  void runClose () {
    this->translateSelectionBy (- this->movement.delta ());

    ActionUnit unit;

    this->entities.caseOf <void> ([this,&unit] (MeshPtrList& ms) {
      for (WingedMesh* m : ms) {
        unit.add <ActionModifyWMesh> (*m).translate (*m, this->movement.delta ());
      }
    });

    if (unit.isEmpty () == false) {
      State::history ().addUnit (std::move (unit));
    }
  }

  ToolResponse runMouseMoveEvent (QMouseEvent& e) {
    glm::vec3 previousDelta = this->movement.delta ();

    if (this->movement.byMouseEvent (e)) {
      this->translateSelectionBy (this->movement.delta () - previousDelta);
      return ToolResponse::Redraw;
    }
    else {
      return ToolResponse::None;
    }
  }
};

DELEGATE1_TOOL                        (ToolMove, MovementConstraint)
DELEGATE_TOOL_RUN_CLOSE               (ToolMove)
DELEGATE_TOOL_RUN_MOUSE_MOVE_EVENT    (ToolMove)

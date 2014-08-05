#include <list>
#include <QObject>
#include <glm/glm.hpp>
#include "tool/move.hpp"
#include "tool/movement.hpp"
#include "variant.hpp"
#include "scene.hpp"
#include "state.hpp"
#include "history.hpp"
#include "sphere/mesh.hpp"
#include "winged/mesh.hpp"
#include "selection-mode.hpp"
#include "selection.hpp"
#include "action/translate.hpp"
#include "mesh-type.hpp"
#include "view/tool-menu-parameters.hpp"
#include "view/main-window.hpp"
#include "view/gl-widget.hpp"

namespace {
  typedef Variant <const SphereMeshes, const WingedMeshes> Entities;
};

struct ToolMove::Impl {
  ToolMove*    self;
  Entities     entities;
  ToolMovement movement;

  Impl (ToolMove* s, Movement m) 
    : self     (s) 
    , entities (std::move (Impl::getEntities ()))
    , movement (m, s->menuParameters ().mainWindow ().glWidget ().cursorPosition ())
  {
  }

  static QString toolName (Movement) {
    return QObject::tr ("Move");
  }

  static Entities getEntities () {
          Entities   entities;
          Scene&     scene     = State::scene ();
    const Selection& selection = scene.selection ();

    switch (scene.selectionMode ()) {
      case SelectionMode::Sphere:
        entities.set <const SphereMeshes> (scene.selectedSphereMeshes ());
        break;
      case SelectionMode::Freeform:
        entities.set <const WingedMeshes> (scene.selectedWingedMeshes (MeshType::Freeform));
        break;
      default:
        assert (false);
    }
    return entities;
  }

  void translateSelectionBy (const glm::vec3& t) {
    this->entities.caseOf <void>
      ( [&t] (const SphereMeshes& ms) { for (SphereMesh* m : ms) { m->translate (t); } }
      , [&t] (const WingedMeshes& ms) { for (WingedMesh* m : ms) { m->translate (t); } }
      );
  }

  void runClose () {
    this->runCancel ();

    this->entities.caseOf <void>
      ( [this] (const SphereMeshes& ms) {
          State::history ().add <ActionTranslate> ().translate (ms, this->movement.delta ());
        }
      , [this] (const WingedMeshes& ms) {
          State::history ().add <ActionTranslate> ().translate (ms, this->movement.delta ());
        }
      );
  }

  void runCancel () {
    this->translateSelectionBy (- this->movement.delta ());
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

  ToolResponse runMouseReleaseEvent (QMouseEvent& e) {
    return this->self->closeOrCancelOnClick (e);
  }
};

DELEGATE_BIG3_BASE ( ToolMove, (const ViewToolMenuParameters& p, Movement m)
                   , (this, m), Tool, (p, toolName (m)) )
DELEGATE1_STATIC (QString     , ToolMove, toolName, Movement)
DELEGATE         (void        , ToolMove, runClose)
DELEGATE         (void        , ToolMove, runCancel)
DELEGATE1        (ToolResponse, ToolMove, runMouseMoveEvent, QMouseEvent&)
DELEGATE1        (ToolResponse, ToolMove, runMouseReleaseEvent, QMouseEvent&)

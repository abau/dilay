#include <QButtonGroup>
#include <QMouseEvent>
#include <QRadioButton>
#include <glm/glm.hpp>
#include "action/modify-winged-mesh.hpp"
#include "action/unit.hpp"
#include "config.hpp"
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
#include "view/properties.hpp"
#include "view/tool/menu-parameters.hpp"
#include "view/util.hpp"
#include "winged/mesh.hpp"

namespace {
  typedef Variant <MeshPtrList> Entities;
};

struct ToolMove::Impl {
  ToolMove*        self;
  QButtonGroup&    constraintEdit;
  Entities         entities;
  ToolUtilMovement movement;

  Impl (ToolMove* s, MovementConstraint c) 
    : self           (s) 
    , constraintEdit (*new QButtonGroup)
    , entities       (std::move (Impl::getEntities ()))
    , movement       (c)
  {
    this->setupButtons ();
  }

  void setupButtons () {
    std::vector <QString> labels = { QObject::tr ("x-axis")
                                   , QObject::tr ("y-axis")
                                   , QObject::tr ("z-axis")
                                   , QObject::tr ("xy-plane")
                                   , QObject::tr ("xz-plane")
                                   , QObject::tr ("yz-plane")
                                   , QObject::tr ("camera-plane")
                                   , QObject::tr ("primary plane") };

    int id = 1; // according to documentation, id should be positive
    for (QString& label : labels) {
      QRadioButton& button = ViewUtil::radioButton (label);

      this->constraintEdit.addButton (&button, id);
      this->self->properties ().addWidget (button);
      id++;
    }

    void (QButtonGroup::* buttonReleased)(int) = &QButtonGroup::buttonReleased;
    QObject::connect (&this->constraintEdit, buttonReleased, [this] (int id) {
      switch (id) {
        case 1: this->movement.constraint (MovementConstraint::XAxis);
                break;
        case 2: this->movement.constraint (MovementConstraint::YAxis);
                break;
        case 3: this->movement.constraint (MovementConstraint::ZAxis);
                break;
        case 4: this->movement.constraint (MovementConstraint::XYPlane);
                break;
        case 5: this->movement.constraint (MovementConstraint::XZPlane);
                break;
        case 6: this->movement.constraint (MovementConstraint::YZPlane);
                break;
        case 7: this->movement.constraint (MovementConstraint::CameraPlane);
                break;
        case 8: this->movement.constraint (MovementConstraint::PrimaryPlane);
                break;
        default:
          std::abort ();
      }
      Config::cache <int> ("/cache/tool/move/constraint", id);
    });
    this->constraintEdit.button (Config::get <int> ("/cache/tool/move/constraint", 7))->click ();
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
    const glm::vec3 previousDelta = this->movement.delta ();

    if (e.buttons ().testFlag (Qt::LeftButton) && this->movement.byMouseEvent (e)) {
      this->translateSelectionBy (this->movement.delta () - previousDelta);
      return ToolResponse::Redraw;
    }
    else {
      return ToolResponse::None;
    }
  }

  ToolResponse runMousePressEvent (QMouseEvent& e) {
    return this->runMouseMoveEvent (e);
  }
};

DELEGATE1_TOOL                      (ToolMove, MovementConstraint)
DELEGATE_TOOL_RUN_CLOSE             (ToolMove)
DELEGATE_TOOL_RUN_MOUSE_MOVE_EVENT  (ToolMove)
DELEGATE_TOOL_RUN_MOUSE_PRESS_EVENT (ToolMove)

#include <QButtonGroup>
#include <QMouseEvent>
#include <QRadioButton>
#include <glm/glm.hpp>
#include "action/modify-winged-mesh.hpp"
#include "action/unit.hpp"
#include "config.hpp"
#include "fwd-winged.hpp"
#include "history.hpp"
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
#include "view/tool/tip.hpp"
#include "view/util.hpp"
#include "view/vector-edit.hpp"
#include "winged/mesh.hpp"

namespace {
  typedef Variant <MeshPtrList> Entities;
};

struct ToolMove::Impl {
  ToolMove*        self;
  Entities         entities;
  ToolUtilMovement movement;
  ViewVectorEdit&  deltaEdit;

  Impl (ToolMove* s) 
    : self      (s) 
    , entities  (std::move (Impl::getEntities ()))
    , movement  (MovementConstraint::CameraPlane)
    , deltaEdit (*new ViewVectorEdit ("\u0394",glm::vec3 (0.0f)))
  {
    this->setupProperties ();
    this->setupToolTip    ();
  }

  void setupProperties () {
    QButtonGroup&   constraintEdit (*new QButtonGroup);

    std::vector <QString> labels = { QObject::tr ("X-axis")
                                   , QObject::tr ("Y-axis")
                                   , QObject::tr ("Z-axis")
                                   , QObject::tr ("XY-plane")
                                   , QObject::tr ("XZ-plane")
                                   , QObject::tr ("YZ-plane")
                                   , QObject::tr ("Camera-plane")
                                   , QObject::tr ("Primary plane") };

    int id = 1; // according to documentation, id should be positive
    for (QString& label : labels) {
      QRadioButton& button = ViewUtil::radioButton (label);

      constraintEdit.addButton (&button, id);
      this->self->properties ().addWidget (button);
      id++;
    }
    this->self->properties ().addWidget (this->deltaEdit);

    void (QButtonGroup::* buttonReleased)(int) = &QButtonGroup::buttonReleased;
    QObject::connect (&constraintEdit, buttonReleased, [this] (int id) {
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
      this->setupToolTip ();
      this->self->config ().cache ("constraint", id);
    });
    constraintEdit.button (this->self->config ().get <int> ("constraint", 7))->click ();

    QObject::connect ( &this->deltaEdit, &ViewVectorEdit::vectorEdited
                     , [this] (const glm::vec3& d) 
    {
      this->resetTranslationOfSelection ();
      this->translateSelectionBy        (d);
      this->movement.delta              (d);
      this->self->updateGlWidget        ();
    });
  }

  void setupToolTip () {
    this->self->resetToolTip ();
    this->self->toolTip ().add (ViewToolTip::Button::Left, QObject::tr ("Drag to move"));

    if (this->movement.constraint () != MovementConstraint::CameraPlane) {
      this->self->toolTip ().add ( ViewToolTip::Button::Left, ViewToolTip::Modifier::Shift
                                 , QObject::tr ("Drag to move orthogonally") );
    }
    this->self->showToolTip ();
  }

  static Entities getEntities () {
          Entities   entities;
          Scene&     scene     = State::scene ();
    const Selection& selection = scene.selection ();

    switch (scene.selectionMode ()) {
      case SelectionMode::WingedMesh: {
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

  void resetTranslationOfSelection () {
    this->translateSelectionBy (- this->movement.delta ());
  }

  void translateSelectionBy (const glm::vec3& t) {
    this->entities.caseOf <void>
      ( [&t] (MeshPtrList& ms) { for (WingedMesh* m : ms) { m->translate (t); } }
      );
  }

  void runClose () {
    this->resetTranslationOfSelection ();

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

    if (e.buttons () == Qt::LeftButton && this->movement.byMouseEvent (e)) {
      this->deltaEdit.vector     (this->movement.delta ());
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

DELEGATE_TOOL                       (ToolMove)
DELEGATE_TOOL_RUN_CLOSE             (ToolMove)
DELEGATE_TOOL_RUN_MOUSE_MOVE_EVENT  (ToolMove)
DELEGATE_TOOL_RUN_MOUSE_PRESS_EVENT (ToolMove)

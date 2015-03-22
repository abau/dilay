#include <QAbstractButton>
#include <QButtonGroup>
#include <QMouseEvent>
#include <glm/glm.hpp>
#include "cache.hpp"
#include "history.hpp"
#include "scene.hpp"
#include "state.hpp"
#include "tool/util/movement.hpp"
#include "tools.hpp"
#include "view/properties.hpp"
#include "view/tool-tip.hpp"
#include "view/util.hpp"
#include "winged/mesh.hpp"
#include "winged/face-intersection.hpp"

struct ToolMoveMesh::Impl {
  ToolMoveMesh*    self;
  WingedMesh*      mesh;
  ToolUtilMovement movement;

  Impl (ToolMoveMesh* s) 
    : self      (s) 
    , mesh      (nullptr)
    , movement  (s->state ().camera (), glm::vec3 (0.0f), MovementConstraint::CameraPlane)
  {
    this->setupProperties ();
    this->setupToolTip    ();

    this->self->snapshotScene ();
  }

  void setupProperties () {
    QButtonGroup& constraintEdit = *new QButtonGroup;
    this->self->properties ().body ().add (
        constraintEdit
      , { QObject::tr ("X-axis")
        , QObject::tr ("Y-axis")
        , QObject::tr ("Z-axis")
        , QObject::tr ("XY-plane")
        , QObject::tr ("XZ-plane")
        , QObject::tr ("YZ-plane")
        , QObject::tr ("Camera-plane")
        , QObject::tr ("Primary plane") 
        }
    );

    ViewUtil::connect (constraintEdit, [this] (int id) {
      switch (id) {
        case 0: this->movement.constraint (MovementConstraint::XAxis);
                break;
        case 1: this->movement.constraint (MovementConstraint::YAxis);
                break;
        case 2: this->movement.constraint (MovementConstraint::ZAxis);
                break;
        case 3: this->movement.constraint (MovementConstraint::XYPlane);
                break;
        case 4: this->movement.constraint (MovementConstraint::XZPlane);
                break;
        case 5: this->movement.constraint (MovementConstraint::YZPlane);
                break;
        case 6: this->movement.constraint (MovementConstraint::CameraPlane);
                break;
        case 7: this->movement.constraint (MovementConstraint::PrimaryPlane);
                break;
        default:
          std::abort ();
      }
      this->setupToolTip ();
      this->self->cache ().set ("constraint", id);
    });
    constraintEdit.button (this->self->cache ().get <int> ("constraint", 6))->click ();
  }

  void setupToolTip () {
    ViewToolTip toolTip;
    toolTip.add (ViewToolTip::MouseEvent::Left, QObject::tr ("Drag to move"));

    if (this->movement.constraint () != MovementConstraint::CameraPlane) {
      toolTip.add ( ViewToolTip::MouseEvent::Left, ViewToolTip::Modifier::Shift
                  , QObject::tr ("Drag to move orthogonally") );
    }
    this->self->showToolTip (toolTip);
  }

  ToolResponse runMouseMoveEvent (const QMouseEvent& e) {
    const glm::vec3 previousDelta = this->movement.delta ();

    if (e.buttons () == Qt::LeftButton && this->mesh && this->movement.move (e)) {
      this->mesh->translate  (this->movement.delta () - previousDelta);
      return ToolResponse::Redraw;
    }
    else {
      return ToolResponse::None;
    }
  }

  ToolResponse runMousePressEvent (const QMouseEvent& e) {
    if (e.button () == Qt::LeftButton) {
      WingedFaceIntersection intersection;
      if (this->self->intersectsScene (e, intersection)) {
        this->mesh = &intersection.mesh ();
        this->movement.resetPosition (intersection.position ());
      }
    }
    return ToolResponse::None;
  }

  ToolResponse runMouseReleaseEvent (const QMouseEvent& e) {
    if (e.button () == Qt::LeftButton && this->mesh) {
      this->mesh->normalize  ();
      this->mesh->bufferData ();
      this->mesh = nullptr;
    }
    return ToolResponse::None;
  }
};

DELEGATE_TOOL                         (ToolMoveMesh)
DELEGATE_TOOL_RUN_MOUSE_MOVE_EVENT    (ToolMoveMesh)
DELEGATE_TOOL_RUN_MOUSE_PRESS_EVENT   (ToolMoveMesh)
DELEGATE_TOOL_RUN_MOUSE_RELEASE_EVENT (ToolMoveMesh)

#include <QAbstractButton>
#include <QButtonGroup>
#include <QFrame>
#include <glm/glm.hpp>
#include "cache.hpp"
#include "tool/sculpt/behaviors.hpp"
#include "tools.hpp"
#include "view/properties.hpp"
#include "view/util.hpp"

struct ToolSculpt::Impl {
  ToolSculpt*                          self;
  std::unique_ptr <ToolSculptBehavior> behavior;
  QButtonGroup&                        behaviorEdit;

  Impl (ToolSculpt* s)
    : self         (s)
    , behaviorEdit (*new QButtonGroup)
  {
    this->setupProperties ();
  }

  void setupProperties () {
    this->self->properties ().header ().add (
        this->behaviorEdit
      , { QObject::tr ("Carve center"), QObject::tr ("Carve perimeter")
        , QObject::tr ("Drag"), QObject::tr ("Smooth"), QObject::tr ("Grab")
        }
    );
    ViewUtil::connect (this->behaviorEdit, [this] (int id) {
      switch (id) {
        case 0: setBehavior <ToolSculptCarveCenter> ();
                break;
        case 1: setBehavior <ToolSculptCarvePerimeter> ();
                break;
        case 2: setBehavior <ToolSculptDrag> ();
                break;
        case 3: setBehavior <ToolSculptSmooth> ();
                break;
        case 4: setBehavior <ToolSculptGrab> ();
                break;
        default:
          std::abort ();
      }
      this->self->cache ().set ("behavior", id);
    });
    this->behaviorEdit.button (this->self->cache ().get <int> ("behavior", 0))->click ();

    this->self->properties ().header ().add (ViewUtil::horizontalLine ());
  }

  template <typename T>
  void setBehavior () {
    this->self->properties ().body ().reset ();

    this->behavior.reset (new T (this->self->cache (), this->self->state ()));
    this->behavior->setupBrush      ();
    this->behavior->setupCursor     (this->self->cursorPosition ());
    this->behavior->setupProperties (this->self->properties ().body ());

    this->self->resetToolTip     ();
    this->behavior->setupToolTip (this->self->toolTip ());
    this->self->showToolTip      ();
  }

  void runRender () const {
    this->behavior->render ();
  }

  ToolResponse runMouseMoveEvent (const QMouseEvent& e) {
    this->behavior->mouseMoveEvent (e);
    return ToolResponse::Redraw;
  }

  ToolResponse runMousePressEvent (const QMouseEvent& e) {
    this->behavior->mousePressEvent (e);
    return ToolResponse::Redraw;
  }

  ToolResponse runMouseReleaseEvent (const QMouseEvent& e) {
    this->behavior->mouseReleaseEvent (e);
    return ToolResponse::Redraw;
  }

  ToolResponse runWheelEvent (const QWheelEvent& e) {
    this->behavior->mouseWheelEvent (e);
    return ToolResponse::Redraw;
  }

  void runClose () {
    this->behavior->close ();
  }
};

DELEGATE_TOOL                         (ToolSculpt)
DELEGATE_TOOL_RUN_RENDER              (ToolSculpt)
DELEGATE_TOOL_RUN_MOUSE_MOVE_EVENT    (ToolSculpt)
DELEGATE_TOOL_RUN_MOUSE_PRESS_EVENT   (ToolSculpt)
DELEGATE_TOOL_RUN_MOUSE_RELEASE_EVENT (ToolSculpt)
DELEGATE_TOOL_RUN_MOUSE_WHEEL_EVENT   (ToolSculpt)
DELEGATE_TOOL_RUN_CLOSE               (ToolSculpt)

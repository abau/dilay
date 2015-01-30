#include <QDoubleSpinBox>
#include <QMouseEvent>
#include <glm/glm.hpp>
#include "scene.hpp"
#include "sculpt-brush.hpp"
#include "state.hpp"
#include "tool/sculpt/behaviors.hpp"
#include "tools.hpp"
#include "view/cursor.hpp"
#include "view/tool/tip.hpp"
#include "view/util.hpp"
#include "winged/mesh.hpp"

struct ToolSculpt::Impl {
  ToolSculpt*                          self;
  std::unique_ptr <ToolSculptBehavior> behavior;

  Impl (ToolSculpt* s) 
    : self     (s) 
  {
    this->setBehavior <ToolSculptCarve> ();
    this->behavior->mouseMoveEvent (this->self->cursorPosition (), false);
  }

  template <typename T>
  void setBehavior () {
    this->behavior.reset (new T (this->self->config (), this->self->state ()));
    this->behavior->setupBrush      ();
    this->behavior->setupProperties (this->self->properties ());

    this->self->resetToolTip     ();
    this->behavior->setupToolTip (this->self->toolTip ());
    this->self->toolTip ().add   ( ViewToolTip::MouseEvent::Wheel, ViewToolTip::Modifier::Shift
                                 , QObject::tr ("Change radius") );
    this->self->showToolTip      ();
  }

  void runRender () {
    this->behavior->cursor ().render (this->self->state ().camera ());
  }

  ToolResponse runMouseMoveEvent (QMouseEvent& e) {
    this->behavior->mouseMoveEvent ( ViewUtil::toIVec2 (e)
                                   , e.buttons () == Qt::LeftButton );
    return ToolResponse::Redraw;
  }

  ToolResponse runMousePressEvent (QMouseEvent& e) {
    if (e.button () == Qt::LeftButton) {
      this->behavior->mouseLeftPressEvent (ViewUtil::toIVec2 (e));
      return ToolResponse::Redraw;
    }
    else {
      return ToolResponse::None;
    }
  }

  ToolResponse runMouseReleaseEvent (QMouseEvent& e) {
    if (e.button () == Qt::LeftButton) {
      this->behavior->brush ().resetPosition ();
      this->behavior->addActionsToHistory ();
      return ToolResponse::Redraw;
    }
    return ToolResponse::None;
  }

  ToolResponse runWheelEvent (QWheelEvent& e) {
    if (e.orientation () == Qt::Vertical && e.modifiers ().testFlag (Qt::ShiftModifier)) {
      if (e.delta () > 0) {
        this->behavior->radiusEdit ().stepUp ();
      }
      else {
        this->behavior->radiusEdit ().stepDown ();
      }
      ViewUtil::deselect (this->behavior->radiusEdit ());
      return ToolResponse::Redraw;
    }
    return ToolResponse::None;
  }

  void runClose () {
    this->behavior->addActionsToHistory ();
  }
};

DELEGATE_TOOL                         (ToolSculpt)
DELEGATE_TOOL_RUN_RENDER              (ToolSculpt)
DELEGATE_TOOL_RUN_MOUSE_MOVE_EVENT    (ToolSculpt)
DELEGATE_TOOL_RUN_MOUSE_PRESS_EVENT   (ToolSculpt)
DELEGATE_TOOL_RUN_MOUSE_RELEASE_EVENT (ToolSculpt)
DELEGATE_TOOL_RUN_MOUSE_WHEEL_EVENT   (ToolSculpt)
DELEGATE_TOOL_RUN_CLOSE               (ToolSculpt)

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

  Impl (ToolSculpt* s) : self (s) {
    this->setBehavior <ToolSculptDrag> ();
    this->behavior->mouseMoveEvent (this->self->cursorPosition (), false);
  }

  template <typename T>
  void setBehavior () {
    this->behavior.reset (new T (this->self->config (), this->self->state ()));
    this->behavior->setupBrushAndCursor ();
    this->behavior->setupProperties     (this->self->properties ());

    this->self->resetToolTip     ();
    this->behavior->setupToolTip (this->self->toolTip ());
    this->self->showToolTip      ();
  }

  void runRender () const {
    this->behavior->render ();
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
      this->behavior->mouseLeftReleaseEvent (ViewUtil::toIVec2 (e));
      return ToolResponse::Redraw;
    }
    return ToolResponse::None;
  }

  ToolResponse runWheelEvent (QWheelEvent& e) {
    if (e.orientation () == Qt::Vertical && e.modifiers ().testFlag (Qt::ShiftModifier)) {
      this->behavior->mouseWheelEvent (e.delta () > 0);
      return ToolResponse::Redraw;
    }
    return ToolResponse::None;
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

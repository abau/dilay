#include <QDoubleSpinBox>
#include <QMouseEvent>
#include <glm/glm.hpp>
#include "action/sculpt.hpp"
#include "action/unit.hpp"
#include "history.hpp"
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
  std::unique_ptr <ActionUnit>         actions;

  Impl (ToolSculpt* s) 
    : self     (s) 
    , actions  (new ActionUnit) 
  {
    this->setBehavior <ToolSculptCarve> ();
    this->behavior->mouseMoveEvent (this->self->state (), this->self->cursorPosition (), false);
  }

  template <typename T>
  void setBehavior () {
    this->behavior.reset (new T (this->self->config ()));
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
    const bool leftButton  = e.buttons () == Qt::LeftButton;
    const bool doCarve     = this->behavior->mouseMoveEvent ( this->self->state ()
                                                            , ViewUtil::toIVec2 (e)
                                                            , leftButton );
    if (leftButton && doCarve) {
      this->actions->add <ActionSculpt, WingedMesh> 
        ( this->self->state ().scene ()
        , this->behavior->brush ().meshRef ()
        ).run (this->behavior->brush ());
    }
    return ToolResponse::Redraw;
  }

  ToolResponse runMouseReleaseEvent (QMouseEvent& e) {
    if (e.button () == Qt::LeftButton && this->actions->isEmpty () == false) {
      this->self->state ().history ().addUnit (std::move (*this->actions));
      this->actions.reset (new ActionUnit ());
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
    if (this->actions->isEmpty () == false) {
      this->self->state ().history ().addUnit (std::move (*this->actions));
    }
  }
};

DELEGATE_TOOL                         (ToolSculpt)
DELEGATE_TOOL_RUN_RENDER              (ToolSculpt)
DELEGATE_TOOL_RUN_MOUSE_MOVE_EVENT    (ToolSculpt)
DELEGATE_TOOL_RUN_MOUSE_RELEASE_EVENT (ToolSculpt)
DELEGATE_TOOL_RUN_MOUSE_WHEEL_EVENT   (ToolSculpt)
DELEGATE_TOOL_RUN_CLOSE               (ToolSculpt)

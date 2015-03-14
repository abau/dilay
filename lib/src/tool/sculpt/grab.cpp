#include <QMouseEvent>
#include <glm/glm.hpp>
#include "tools.hpp"
#include "sculpt-brush.hpp"
#include "state.hpp"
#include "tool/util/movement.hpp"
#include "view/properties.hpp"
#include "view/tool-tip.hpp"

struct ToolSculptGrab::Impl {
  ToolSculptGrab*  self;
  ToolUtilMovement movement;

  Impl (ToolSculptGrab* s) 
    : self (s)
    , movement ( this->self->state ().camera ()
               , glm::vec3 (0.0f)
               , MovementConstraint::Explicit )
  {}

  void runSetupBrush (SculptBrush& brush) {
    brush.useLastPosition   (true);
    brush.useIntersection   (true);
    brush.linearStep        (true);
  }

  void runSetupCursor (ViewCursor&) {}

  void runSetupProperties (ViewPropertiesPart&) {}

  void runSetupToolTip (ViewToolTip& toolTip) {
    toolTip.add (ViewToolTip::MouseEvent::Left, QObject::tr ("Drag to sculpt"));
  }

  ToolResponse runMouseMoveEvent (const QMouseEvent& e) {
    this->self->draglikeStroke (e, this->movement);
    return ToolResponse::Redraw;
  }

  ToolResponse runMousePressEvent (const QMouseEvent& e) {
    this->self->initializeDraglikeStroke (e, this->movement);
    return ToolResponse::Redraw;
  }
};

DELEGATE_TOOL_SCULPT (ToolSculptGrab)

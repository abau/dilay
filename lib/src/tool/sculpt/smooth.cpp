#include <QObject>
#include "sculpt-brush.hpp"
#include "tools.hpp"
#include "view/tool-tip.hpp"

struct ToolSculptSmooth::Impl {
  ToolSculptSmooth* self;

  Impl (ToolSculptSmooth* s) 
    : self (s) 
  {}

  void runSetupBrush (SculptBrush& brush) {
    brush.useIntersection (true);
    brush.intensityFactor (0.0f);
  }

  void runSetupCursor (ViewCursor&) {}

  void runSetupProperties (ViewPropertiesPart&) {}

  void runSetupToolTip (ViewToolTip& toolTip) {
    toolTip.add ( ViewToolTip::MouseEvent::Left, QObject::tr ("Drag to smooth"));
  }

  ToolResponse runMouseMoveEvent (const QMouseEvent& e) {
    this->self->carvelikeStroke (e);
    return ToolResponse::Redraw;
  }

  ToolResponse runMousePressEvent (const QMouseEvent& e) {
    this->self->carvelikeStroke (e);
    return ToolResponse::Redraw;
  }
};

DELEGATE_TOOL_SCULPT (ToolSculptSmooth)

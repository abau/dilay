#include <QObject>
#include "sculpt-brush.hpp"
#include "tools.hpp"
#include "view/cursor.hpp"
#include "view/tool-tip.hpp"

struct ToolSculptSmooth::Impl {
  ToolSculptSmooth*  self;
  ViewCursor         cursor;

  Impl (ToolSculptSmooth* s) 
    : self (s) 
  {}

  void runSetupBrush (SculptBrush& brush) {
    brush.useIntersection (true);
    brush.intensityFactor (0.0f);
  }

  void runSetupCursor () {}

  void runSetupProperties (ViewPropertiesPart&) {}

  void runSetupToolTip (ViewToolTip& toolTip) {
    toolTip.add ( ViewToolTip::MouseEvent::Left, QObject::tr ("Drag to smooth"));
  }

  ToolResponse carve (const QMouseEvent& e) {
    if (this->self->updateBrushAndCursorByIntersection (e)) {
      this->self->sculpt ();
    }
    return ToolResponse::Redraw;
  }

  ToolResponse runMouseMoveEvent (const QMouseEvent& e) {
    return this->carve (e);
  }

  ToolResponse runMousePressEvent (const QMouseEvent& e) {
    return this->carve (e);
  }
};

DELEGATE_TOOL_SCULPT (ToolSculptSmooth)

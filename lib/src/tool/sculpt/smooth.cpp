#include <QObject>
#include "sculpt-brush/nothing.hpp"
#include "tool/sculpt/behaviors.hpp"
#include "view/tool/tip.hpp"

struct ToolSculptSmooth::Impl {
  ToolSculptSmooth*  self;
  SculptBrushNothing brush;

  Impl (ToolSculptSmooth* s) 
    : self (s) 
  {}

  void runSetupBrush () {}

  void runSetupProperties (ViewPropertiesPart&) {}

  void runSetupToolTip (ViewToolTip& toolTip) {
    toolTip.add ( ViewToolTip::MouseEvent::Left, QObject::tr ("Drag to smooth"));
  }

  void carve (const QMouseEvent& e) {
    if (this->self->updateBrushByIntersection (e)) {
      this->self->sculpt ();
    }
  }

  void runMouseMoveEvent (const QMouseEvent& e) {
    this->carve (e);
  }

  void runMousePressEvent (const QMouseEvent& e) {
    this->carve (e);
  }
};

DELEGATE_TOOL_SCULPT_BEHAVIOR (ToolSculptSmooth)

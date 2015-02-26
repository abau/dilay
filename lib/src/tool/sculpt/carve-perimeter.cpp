#include <QSpinBox>
#include <glm/glm.hpp>
#include "config.hpp"
#include "sculpt-brush/carve.hpp"
#include "tool/sculpt/behaviors.hpp"
#include "view/cursor/inner-radius.hpp"
#include "view/properties.hpp"
#include "view/tool/tip.hpp"
#include "view/util.hpp"

struct ToolSculptCarvePerimeter::Impl {
  ToolSculptCarvePerimeter* self;
  SculptBrushCarve          brush;
  ViewCursorInnerRadius     cursor;

  Impl (ToolSculptCarvePerimeter* s) : self (s) {}

  void runSetupBrush () {
    this->brush.innerRadiusFactor (this->self->config ().get <float> ("inner-radius-factor", 0.5f));
    this->brush.carvePerimeter    (true);
  }

  void runSetupCursor () {
    this->cursor.innerRadiusFactor (this->brush.innerRadiusFactor ());
  }

  void runSetupProperties (ViewPropertiesPart& properties) {
    QDoubleSpinBox& innerRadiusEdit = ViewUtil::spinBox ( 0.0f, this->brush.innerRadiusFactor ()
                                                        , 1.0f, 0.1f );
    ViewUtil::connect (innerRadiusEdit, [this] (float f) {
      this->brush.innerRadiusFactor  (f);
      this->cursor.innerRadiusFactor (f);
      this->self->config ().cache ("inner-radius-factor", f);
    });
    properties.add (QObject::tr ("Inner radius"), innerRadiusEdit);
  }

  void runSetupToolTip (ViewToolTip& toolTip) {
    toolTip.add (ViewToolTip::MouseEvent::Left, QObject::tr ("Drag to carve"));
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

DELEGATE_TOOL_SCULPT_BEHAVIOR (ToolSculptCarvePerimeter)

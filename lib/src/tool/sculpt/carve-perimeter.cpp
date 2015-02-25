#include <QSpinBox>
#include <glm/glm.hpp>
#include "config.hpp"
#include "sculpt-brush/carve.hpp"
#include "tool/sculpt/behaviors.hpp"
#include "view/cursor.hpp"
#include "view/properties.hpp"
#include "view/tool/tip.hpp"
#include "view/util.hpp"

struct ToolSculptCarvePerimeter::Impl {
  ToolSculptCarvePerimeter* self;
  ViewCursor                cursor;
  SculptBrushCarve          brush;

  Impl (ToolSculptCarvePerimeter* s) : self (s) {}

  void runSetupBrush () {
    this->brush.flatness       (this->self->config ().get <float> ("flatness", 0.5f));
    this->brush.carvePerimeter (true);
  }

  void runSetupProperties (ViewPropertiesPart& properties) {
    QDoubleSpinBox& flatnessEdit = ViewUtil::spinBox (0.0f, this->brush.flatness (), 1.0f, 0.1f);
    ViewUtil::connect (flatnessEdit, [this] (float f) {
      this->brush.flatness (f);
      this->self->config ().cache ("flatness", f);
    });
    properties.add (QObject::tr ("Flatness"), flatnessEdit);
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

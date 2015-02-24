#include <QSpinBox>
#include <glm/glm.hpp>
#include "config.hpp"
#include "sculpt-brush/carve.hpp"
#include "tool/sculpt/behaviors.hpp"
#include "view/properties.hpp"
#include "view/tool/tip.hpp"
#include "view/util.hpp"

struct ToolSculptCarvePerimeter::Impl {
  ToolSculptCarvePerimeter* self;
  SculptBrushCarve          brush;

  Impl (ToolSculptCarvePerimeter* s) : self (s) {}

  void runSetupBrush () {
    this->brush.flatness       (this->self->config ().get <int> ("flatness", 4));
    this->brush.carvePerimeter (true);
  }

  void runSetupProperties (ViewPropertiesPart& properties) {
    QSpinBox& flatnessEdit = ViewUtil::spinBox (3, this->brush.flatness (), 1000, 10);
    ViewUtil::connect (flatnessEdit, [this] (int f) {
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

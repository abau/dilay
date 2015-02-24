#include <QCheckBox>
#include <QDoubleSpinBox>
#include <QMouseEvent>
#include <glm/glm.hpp>
#include "config.hpp"
#include "sculpt-brush/carve.hpp"
#include "tool/sculpt/behaviors.hpp"
#include "view/properties.hpp"
#include "view/tool/tip.hpp"
#include "view/util.hpp"

struct ToolSculptCarveCenter::Impl {
  ToolSculptCarveCenter* self;
  SculptBrushCarve brush;

  Impl (ToolSculptCarveCenter* s) : self (s) {}

  void runSetupBrush () {
    this->brush.intensityFactor (this->self->config ().get <float> ("intensity-factor", 0.03f));
    this->brush.flatness        (this->self->config ().get <int>   ("flatness"        , 4));
    this->brush.invert          (this->self->config ().get <bool>  ("invert"          , false));
  }

  void runSetupProperties (ViewPropertiesPart& properties) {
    QDoubleSpinBox& intensityEdit = ViewUtil::spinBox ( 0.0f, this->brush.intensityFactor ()
                                                      , 0.1f, 0.01f );
    ViewUtil::connect (intensityEdit, [this] (float i) {
      this->brush.intensityFactor (i);
      this->self->config ().cache ("intensity", i);
    });
    properties.add (QObject::tr ("Intensity"), intensityEdit);

    QSpinBox& flatnessEdit = ViewUtil::spinBox (3, this->brush.flatness (), 1000, 10);
    ViewUtil::connect (flatnessEdit, [this] (int f) {
      this->brush.flatness (f);
      this->self->config ().cache ("flatness", f);
    });
    properties.add (QObject::tr ("Flatness"), flatnessEdit);

    QCheckBox& invertEdit = ViewUtil::checkBox (QObject::tr ("Invert"), this->brush.invert ());
    ViewUtil::connect (invertEdit, [this] (bool i) {
      this->brush.invert (i);
      this->self->config ().cache ("invert", i);
    });
    properties.add (invertEdit);
  }

  void runSetupToolTip (ViewToolTip& toolTip) {
    toolTip.add ( ViewToolTip::MouseEvent::Left, QObject::tr ("Drag to carve"));
    toolTip.add ( ViewToolTip::MouseEvent::Left
                , ViewToolTip::Modifier::Shift, QObject::tr ("Drag to carve inverted"));
  }

  void carve (const QMouseEvent& e) {
    if (this->self->updateBrushByIntersection (e)) {
      if (e.modifiers () == Qt::ShiftModifier) {
        this->brush.toggleInvert ();
        this->self->sculpt ();
        this->brush.toggleInvert ();
      }
      else {
        this->self->sculpt ();
      }
    }
  }

  void runMouseMoveEvent (const QMouseEvent& e) {
    this->carve (e);
  }

  void runMousePressEvent (const QMouseEvent& e) {
    this->carve (e);
  }
};

DELEGATE_TOOL_SCULPT_BEHAVIOR (ToolSculptCarveCenter)

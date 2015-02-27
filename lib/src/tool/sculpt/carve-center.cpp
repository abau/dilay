#include <QCheckBox>
#include <QDoubleSpinBox>
#include <QMouseEvent>
#include <glm/glm.hpp>
#include "cache.hpp"
#include "sculpt-brush/carve.hpp"
#include "tool/sculpt/behaviors.hpp"
#include "view/cursor/inner-radius.hpp"
#include "view/properties.hpp"
#include "view/tool/tip.hpp"
#include "view/util.hpp"

struct ToolSculptCarveCenter::Impl {
  ToolSculptCarveCenter* self;
  SculptBrushCarve       brush;
  ViewCursorInnerRadius  cursor;

  Impl (ToolSculptCarveCenter* s) : self (s) {}

  void runSetupBrush () {
    this->brush.intensityFactor   (this->self->cache ().get <float> ("intensity-factor"   , 0.03f));
    this->brush.innerRadiusFactor (this->self->cache ().get <float> ("inner-radius-factor", 0.5f));
    this->brush.invert            (this->self->cache ().get <bool>  ("invert"             , false));
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
      this->self->cache ().set ("inner-radius-factor", f);
    });
    properties.add (QObject::tr ("Inner radius"), innerRadiusEdit);

    QDoubleSpinBox& intensityEdit = ViewUtil::spinBox ( 0.0f, this->brush.intensityFactor ()
                                                      , 0.1f, 0.01f );
    ViewUtil::connect (intensityEdit, [this] (float i) {
      this->brush.intensityFactor (i);
      this->self->cache ().set ("intensity", i);
    });
    properties.add (QObject::tr ("Intensity"), intensityEdit);

    QCheckBox& invertEdit = ViewUtil::checkBox (QObject::tr ("Invert"), this->brush.invert ());
    ViewUtil::connect (invertEdit, [this] (bool i) {
      this->brush.invert (i);
      this->self->cache ().set ("invert", i);
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

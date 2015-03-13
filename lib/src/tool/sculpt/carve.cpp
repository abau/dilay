#include <QCheckBox>
#include <QDoubleSpinBox>
#include <QMouseEvent>
#include <glm/glm.hpp>
#include "cache.hpp"
#include "tools.hpp"
#include "sculpt-brush.hpp"
#include "view/cursor.hpp"
#include "view/properties.hpp"
#include "view/tool-tip.hpp"
#include "view/util.hpp"

struct ToolSculptCarve::Impl {
  ToolSculptCarve* self;

  Impl (ToolSculptCarve* s) : self (s) {}

  void runSetupBrush (SculptBrush& brush) {
    brush.intensityFactor   (this->self->cache ().get <float> ("intensity-factor"   , 0.03f));
    brush.innerRadiusFactor (this->self->cache ().get <float> ("inner-radius-factor", 0.5f));
    brush.invert            (this->self->cache ().get <bool>  ("invert"             , false));
  }

  void runSetupCursor (ViewCursor& cursor) {
    cursor.hasInnerRadius    (true);
    cursor.innerRadiusFactor (this->self->brush ().innerRadiusFactor ());
  }

  void runSetupProperties (ViewPropertiesPart& properties) {
    SculptBrush& brush  = this->self->brush ();
    ViewCursor&  cursor = this->self->cursor ();

    QDoubleSpinBox& innerRadiusEdit = ViewUtil::spinBox ( 0.0f, brush.innerRadiusFactor ()
                                                        , 1.0f, 0.1f );
    ViewUtil::connect (innerRadiusEdit, [this,&brush,&cursor] (float f) {
      brush .innerRadiusFactor (f);
      cursor.innerRadiusFactor (f);
      this->self->cache ().set ("inner-radius-factor", f);
    });
    properties.add (QObject::tr ("Inner radius"), innerRadiusEdit);

    QDoubleSpinBox& intensityEdit = ViewUtil::spinBox ( 0.0f, brush.intensityFactor ()
                                                      , 0.1f, 0.01f );
    ViewUtil::connect (intensityEdit, [this,&brush] (float i) {
      brush.intensityFactor (i);
      this->self->cache ().set ("intensity", i);
    });
    properties.add (QObject::tr ("Intensity"), intensityEdit);

    QCheckBox& invertEdit = ViewUtil::checkBox (QObject::tr ("Invert"), brush.invert ());
    ViewUtil::connect (invertEdit, [this,&brush] (bool i) {
      brush.invert (i);
      this->self->cache ().set ("invert", i);
    });
    properties.add (invertEdit);
  }

  void runSetupToolTip (ViewToolTip& toolTip) {
    toolTip.add ( ViewToolTip::MouseEvent::Left, QObject::tr ("Drag to sculpt"));
    toolTip.add ( ViewToolTip::MouseEvent::Left
                , ViewToolTip::Modifier::Shift, QObject::tr ("Drag to sculpt inverted"));
  }

  ToolResponse carve (const QMouseEvent& e) {
    if (this->self->updateBrushAndCursorByIntersection (e)) {
      if (e.modifiers () == Qt::ShiftModifier) {
        this->self->brush ().toggleInvert ();
        this->self->sculpt ();
        this->self->brush ().toggleInvert ();
      }
      else {
        this->self->sculpt ();
      }
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

DELEGATE_TOOL_SCULPT (ToolSculptCarve)

#include <QDoubleSpinBox>
#include "cache.hpp"
#include "tools.hpp"
#include "sculpt-brush.hpp"
#include "view/cursor.hpp"
#include "view/properties.hpp"
#include "view/tool-tip.hpp"
#include "view/util.hpp"

struct ToolSculptFlatten::Impl {
  ToolSculptFlatten* self;

  Impl (ToolSculptFlatten* s) : self (s) {}

  void runSetupBrush (SculptBrush& brush) {
    brush.mode            (SculptBrush::Mode::Flatten);
    brush.intensityFactor (this->self->cache ().get <float> ("intensity-factor", 0.5f));
  }

  void runSetupCursor (ViewCursor&) {}

  void runSetupProperties (ViewPropertiesPart& properties) {
    SculptBrush& brush = this->self->brush ();

    QDoubleSpinBox& intensityEdit = ViewUtil::spinBox ( 0.1f, brush.intensityFactor ()
                                                      , 1.0f, 0.1f );
    ViewUtil::connect (intensityEdit, [this,&brush] (float i) {
      brush.intensityFactor (i);
      this->self->cache ().set ("intensity", i);
    });
    properties.add (QObject::tr ("Intensity"), intensityEdit);
  }

  void runSetupToolTip (ViewToolTip& toolTip) {
    toolTip.add (ViewToolTip::MouseEvent::Left, QObject::tr ("Drag to flatten"));
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

DELEGATE_TOOL_SCULPT (ToolSculptFlatten)

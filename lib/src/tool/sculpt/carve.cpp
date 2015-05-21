#include <QCheckBox>
#include "cache.hpp"
#include "sculpt-brush.hpp"
#include "tools.hpp"
#include "view/double-slider.hpp"
#include "view/properties.hpp"
#include "view/tool-tip.hpp"
#include "view/util.hpp"

struct ToolSculptCarve::Impl {
  ToolSculptCarve* self;

  Impl (ToolSculptCarve* s) : self (s) {}

  void runSetupBrush (SculptBrush& brush) {
    auto& params = brush.parameters <SBCarveParameters> ();

    params.intensity  (this->self->cache ().get <float> ("intensity",  0.03f));
    params.smoothness (this->self->cache ().get <float> ("smoothness", 0.5f));
    params.invert     (this->self->cache ().get <bool>  ("invert",     false));
  }

  void runSetupCursor (ViewCursor&) {}

  void runSetupProperties (ViewPropertiesPart& properties) {
    auto&       params = this->self->brush ().parameters <SBCarveParameters> ();
    ViewCursor& cursor = this->self->cursor ();

    ViewDoubleSlider& smoothnessEdit = ViewUtil::slider ( 0.0f, params.smoothness ()
                                                        , 1.0f, 0.1f );
    ViewUtil::connect (smoothnessEdit, [this,&params,&cursor] (float f) {
      params.smoothness (f);
      this->self->cache ().set ("smoothness", f);
    });
    properties.addStacked (QObject::tr ("Smoothness"), smoothnessEdit);

    ViewDoubleSlider& intensityEdit = ViewUtil::slider ( 0.01f, params.intensity ()
                                                       , 0.1f, 0.01f );
    ViewUtil::connect (intensityEdit, [this,&params] (float i) {
      params.intensity (i);
      this->self->cache ().set ("intensity", i);
    });
    properties.addStacked (QObject::tr ("Intensity"), intensityEdit);

    QCheckBox& invertEdit = ViewUtil::checkBox (QObject::tr ("Invert"), params.invert ());
    ViewUtil::connect (invertEdit, [this,&params] (bool i) {
      params.invert (i);
      this->self->cache ().set ("invert", i);
    });
    properties.add (invertEdit);
  }

  void runSetupToolTip (ViewToolTip& toolTip) {
    toolTip.add ( ViewToolTip::MouseEvent::Left, QObject::tr ("Drag to sculpt"));
    toolTip.add ( ViewToolTip::MouseEvent::Left
                , ViewToolTip::Modifier::Shift, QObject::tr ("Drag to sculpt inverted"));
  }

  bool runMouseEvent (const QMouseEvent& e) {
    const std::function <void ()> toggleInvert = [this] () {
      this->self->brush ().parameters <SBCarveParameters> ().toggleInvert ();
    };
    return this->self->carvelikeStroke (e, false, &toggleInvert);
  }

  void runSculptMouseMoveEvent (const QMouseEvent& e) {
    this->runMouseEvent (e);
  }

  bool runSculptMousePressEvent (const QMouseEvent& e) {
    return this->runMouseEvent (e);
  }
};

DELEGATE_TOOL_SCULPT (ToolSculptCarve)

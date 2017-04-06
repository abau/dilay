/* This file is part of Dilay
 * Copyright © 2015,2016 Alexander Bau
 * Use and redistribute under the terms of the GNU General Public License
 */
#include <QCheckBox>
#include "cache.hpp"
#include "tool/sculpt/util/brush.hpp"
#include "tools.hpp"
#include "view/double-slider.hpp"
#include "view/two-column-grid.hpp"
#include "view/util.hpp"

struct ToolSculptDraw::Impl {
  ToolSculptDraw* self;

  Impl (ToolSculptDraw* s) : self (s) {}

  void runSetupBrush (SculptBrush& brush) {
    auto& params = brush.initParameters <SBDrawParameters> ();

    params.intensity (this->self->cache ().get <float> ("intensity", 0.5f));
    params.invert    (this->self->cache ().get <bool>  ("invert",    false));
    params.flat (this->self->cache ().get <bool>  ("flat", true));
  }

  void runSetupCursor (ViewCursor&) {}

  void runSetupProperties (ViewTwoColumnGrid& properties) {
    auto& params = this->self->brush ().parameters <SBDrawParameters> ();

    ViewDoubleSlider& intensityEdit = ViewUtil::slider (2, 0.0f, params.intensity (), 1.0f);
    ViewUtil::connect (intensityEdit, [this,&params] (float i) {
      params.intensity (i);
      this->self->cache ().set ("intensity", i);
    });
    properties.addStacked (QObject::tr ("Intensity"), intensityEdit);
    this->self->registerSecondarySlider (intensityEdit);

    QCheckBox& invertEdit = ViewUtil::checkBox (QObject::tr ("Invert"), params.invert ());
    ViewUtil::connect (invertEdit, [this,&params] (bool i) {
      params.invert (i);
      this->self->cache ().set ("invert", i);
    });
    properties.add (invertEdit);

    QCheckBox& flatEdit = ViewUtil::checkBox (QObject::tr ("Flat"), params.flat ());
    ViewUtil::connect (flatEdit, [this,&params] (bool f) {
      params.flat (f);
      this->self->cache ().set ("flat", f);
    });
    properties.add (flatEdit);
  }

  void runSetupToolTip (ViewToolTip& toolTip) {
    this->self->addDefaultToolTip        (toolTip, true);
    this->self->addSecSliderWheelToolTip (toolTip, QObject::tr ("Change intensity"));
  }

  bool runSculptPointingEvent (const ViewPointingEvent& e) {
    const std::function <void ()> toggleInvert = [this] () {
      this->self->brush ().parameters <SBDrawParameters> ().toggleInvert ();
    };
    return this->self->drawlikeStroke (e, false, &toggleInvert);
  }
};

DELEGATE_TOOL_SCULPT (ToolSculptDraw)

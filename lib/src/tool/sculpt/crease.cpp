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

struct ToolSculptCrease::Impl {
  ToolSculptCrease* self;

  Impl (ToolSculptCrease* s) : self (s) {}

  void runSetupBrush (SculptBrush& brush) {
    auto& params = brush.initParameters <SBCreaseParameters> ();

    brush.detailFactor (brush.detailFactor () + 0.5f);
    params.intensity   (this->self->cache ().get <float> ("intensity", 0.5f));
    params.invert      (this->self->cache ().get <bool>  ("invert"   , false));
  }

  void runSetupCursor (ViewCursor&) {}

  void runSetupProperties (ViewTwoColumnGrid& properties) {
    auto& params = this->self->brush ().parameters <SBCreaseParameters> ();

    ViewDoubleSlider& intensityEdit = ViewUtil::slider (2, 0.1f, params.intensity (), 0.9f);
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
  }

  void runSetupToolTip (ViewToolTip& toolTip) {
    this->self->addDefaultToolTip        (toolTip, true);
    this->self->addSecSliderWheelToolTip (toolTip, QObject::tr ("Change intensity"));
  }

  bool runSculptPointingEvent (const ViewPointingEvent& e) {
    const std::function <void ()> toggleInvert = [this] () {
      this->self->brush ().parameters <SBCreaseParameters> ().toggleInvert ();
    };
    return this->self->carvelikeStroke (e, true, &toggleInvert);
  }
};

DELEGATE_TOOL_SCULPT (ToolSculptCrease)

/* This file is part of Dilay
 * Copyright © 2015,2016 Alexander Bau
 * Use and redistribute under the terms of the GNU General Public License
 */
#include "cache.hpp"
#include "tool/sculpt/util/brush.hpp"
#include "tools.hpp"
#include "view/double-slider.hpp"
#include "view/two-column-grid.hpp"
#include "view/util.hpp"

struct ToolSculptReduce::Impl
{
  ToolSculptReduce* self;

  Impl (ToolSculptReduce* s)
    : self (s)
  {
  }

  void runSetupBrush (SculptBrush& brush)
  {
    auto& params = brush.initParameters<SBReduceParameters> ();

    params.intensity (this->self->cache ().get<float> ("intensity", 0.5f));
  }

  void runSetupCursor (ViewCursor&)
  {
  }

  void runSetupProperties (ViewTwoColumnGrid& properties)
  {
    auto& params = this->self->brush ().parameters<SBReduceParameters> ();

    ViewDoubleSlider& intensityEdit = ViewUtil::slider (2, 0.1f, params.intensity (), 0.9f);
    ViewUtil::connect (intensityEdit, [this, &params](float i) {
      params.intensity (i);
      this->self->cache ().set ("intensity", i);
    });
    properties.addStacked (QObject::tr ("Intensity"), intensityEdit);
    this->self->registerSecondarySlider (intensityEdit);
  }

  void runSetupToolTip (ViewToolTip& toolTip)
  {
    this->self->addDefaultToolTip (toolTip, false);
    this->self->addSecSliderWheelToolTip (toolTip, QObject::tr ("Change intensity"));
  }

  bool runSculptPointingEvent (const ViewPointingEvent& e)
  {
    return this->self->drawlikeStroke (e, false);
  }
};

DELEGATE_TOOL_SCULPT (ToolSculptReduce)

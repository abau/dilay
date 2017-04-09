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

struct ToolSculptSmooth::Impl
{
  ToolSculptSmooth* self;

  Impl (ToolSculptSmooth* s)
    : self (s)
  {
  }

  void runSetupBrush (SculptBrush& brush)
  {
    auto& params = brush.initParameters<SBSmoothParameters> ();

    params.intensity (this->self->cache ().get<float> ("intensity", 0.5f));
    params.relaxOnly (this->self->cache ().get<bool> ("relax-only", false));
  }

  void runSetupCursor (ViewCursor&)
  {
  }

  void runSetupProperties (ViewTwoColumnGrid& properties)
  {
    auto& params = this->self->brush ().parameters<SBSmoothParameters> ();

    ViewDoubleSlider& intensityEdit = ViewUtil::slider (2, 0.1f, params.intensity (), 1.0f);
    ViewUtil::connect (intensityEdit, [this, &params](float i) {
      params.intensity (i);
      this->self->cache ().set ("intensity", i);
    });
    intensityEdit.setEnabled (!params.relaxOnly ());
    properties.addStacked (QObject::tr ("Intensity"), intensityEdit);
    this->self->registerSecondarySlider (intensityEdit);

    QCheckBox& relaxEdit = ViewUtil::checkBox (QObject::tr ("Relax only"), params.relaxOnly ());
    ViewUtil::connect (relaxEdit, [this, &params, &intensityEdit](bool r) {
      params.relaxOnly (r);
      intensityEdit.setEnabled (!r);
      this->self->cache ().set ("relax-only", r);
    });
    properties.add (relaxEdit);
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

DELEGATE_TOOL_SCULPT (ToolSculptSmooth)

/* This file is part of Dilay
 * Copyright © 2015,2016 Alexander Bau
 * Use and redistribute under the terms of the GNU General Public License
 */
#include <QCheckBox>
#include "cache.hpp"
#include "sculpt-brush.hpp"
#include "tools.hpp"
#include "view/double-slider.hpp"
#include "view/properties.hpp"
#include "view/util.hpp"

struct ToolSculptCarve::Impl {
  ToolSculptCarve* self;

  Impl (ToolSculptCarve* s) : self (s) {}

  void runSetupBrush (SculptBrush& brush) {
    auto& params = brush.parameters <SBCarveParameters> ();

    params.intensity (this->self->cache ().get <float> ("intensity", 0.02f));
    params.invert    (this->self->cache ().get <bool>  ("invert",    false));
    params.inflate   (this->self->cache ().get <bool>  ("inflate",   false));
  }

  void runSetupCursor (ViewCursor&) {}

  void runSetupProperties (ViewPropertiesPart& properties) {
    auto& params = this->self->brush ().parameters <SBCarveParameters> ();

    ViewDoubleSlider& intensityEdit = ViewUtil::slider (3, 0.0f, params.intensity (), 0.05f);
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

    QCheckBox& inflateEdit = ViewUtil::checkBox (QObject::tr ("Inflate"), params.inflate ());
    ViewUtil::connect (inflateEdit, [this,&params] (bool i) {
      params.inflate (i);
      this->self->cache ().set ("inflate", i);
    });
    properties.add (inflateEdit);
  }

  void runSetupToolTip (ViewToolTip& toolTip) {
    this->self->addDefaultToolTip (toolTip, true);
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

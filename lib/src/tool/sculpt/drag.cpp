/* This file is part of Dilay
 * Copyright © 2015,2016 Alexander Bau
 * Use and redistribute under the terms of the GNU General Public License
 */
#include <QCheckBox>
#include "cache.hpp"
#include "sculpt-brush.hpp"
#include "state.hpp"
#include "tool/util/movement.hpp"
#include "tools.hpp"
#include "view/double-slider.hpp"
#include "view/pointing-event.hpp"
#include "view/properties.hpp"
#include "view/util.hpp"

struct ToolSculptDrag::Impl {
  ToolSculptDrag*  self;
  ToolUtilMovement movement;

  Impl (ToolSculptDrag* s) 
    : self (s)
    , movement (this->self->state ().camera (), MovementConstraint::CameraPlane)
  {
    if (this->self->cache ().get <bool> ("along-primary-plane", false)) {
      this->movement.constraint (MovementConstraint::PrimaryPlane);
    }
  }

  void runSetupBrush (SculptBrush& brush) {
    auto& params = brush.parameters <SBDraglikeParameters> ();

    params.smoothness       (this->self->cache ().get <float> ("smoothness", 0.5f));
    params.linearStep       (false);
    params.discardBackfaces (this->self->cache ().get <bool>  ("discard-backfaces", false));
  }

  void runSetupCursor (ViewCursor&) {}

  void runSetupProperties (ViewTwoColumnGrid& properties) {
    auto& params = this->self->brush ().parameters <SBDraglikeParameters> ();

    ViewDoubleSlider& smoothnessEdit = ViewUtil::slider (2, 0.0f, params.smoothness (), 1.0f);
    ViewUtil::connect (smoothnessEdit, [this,&params] (float f) {
      params.smoothness (f);
      this->self->cache ().set ("smoothness", f);
    });
    properties.addStacked (QObject::tr ("Smoothness"), smoothnessEdit);

    QCheckBox& primPlaneEdit = ViewUtil::checkBox 
      ( QObject::tr ("Along primary plane")
      , this->movement.constraint () == MovementConstraint::PrimaryPlane
      );
    ViewUtil::connect (primPlaneEdit, [this] (bool p) {
      this->movement.constraint ( p ? MovementConstraint::PrimaryPlane
                                    : MovementConstraint::CameraPlane );
      this->self->cache ().set ("along-primary-plane", p);
    });
    properties.add (primPlaneEdit);

    QCheckBox& discardEdit = ViewUtil::checkBox ( QObject::tr ("Discard backfaces")
                                                , params.discardBackfaces () );
    ViewUtil::connect (discardEdit, [this,&params] (bool d) {
      params.discardBackfaces (d);
      this->self->cache ().set ("discard-backfaces", d);
    });
    properties.add (discardEdit);
  }

  void runSetupToolTip (ViewToolTip& toolTip) {
    this->self->addDefaultToolTip (toolTip, false);
  }

  bool runSculptPointingEvent (const ViewPointingEvent& e) {
    return e.pressEvent () ? this->self->initializeDraglikeStroke (e, this->movement)
                           : this->self->draglikeStroke (e, this->movement);
  }
};

DELEGATE_TOOL_SCULPT (ToolSculptDrag)

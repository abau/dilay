/* This file is part of Dilay
 * Copyright © 2015-2017 Alexander Bau
 * Use and redistribute under the terms of the GNU General Public License
 */
#include <QCheckBox>
#include "cache.hpp"
#include "dynamic/mesh-intersection.hpp"
#include "primitive/plane.hpp"
#include "state.hpp"
#include "tool/sculpt/util/brush.hpp"
#include "tool/util/movement.hpp"
#include "tools.hpp"
#include "view/cursor.hpp"
#include "view/double-slider.hpp"
#include "view/pointing-event.hpp"
#include "view/two-column-grid.hpp"
#include "view/util.hpp"

struct ToolSculptFlatten::Impl
{
  ToolSculptFlatten* self;
  ToolUtilMovement   movement;

  Impl (ToolSculptFlatten* s)
    : self (s)
    , movement (this->self->state ().camera (), glm::vec3 (0.0f))
  {
  }

  void runSetupBrush (SculptBrush& brush)
  {
    auto& params = brush.initParameters<SBFlattenParameters> ();

    params.intensity (this->self->cache ().get<float> ("intensity", 0.5f));
    params.lockPlane (this->self->cache ().get<bool> ("lock-plane", false));
  }

  void runSetupCursor (ViewCursor&) {}

  void runSetupProperties (ViewTwoColumnGrid& properties)
  {
    auto& params = this->self->brush ().parameters<SBFlattenParameters> ();

    ViewDoubleSlider& intensityEdit = ViewUtil::slider (2, 0.1f, params.intensity (), 1.0f);
    ViewUtil::connect (intensityEdit, [this, &params](float i) {
      params.intensity (i);
      this->self->cache ().set ("intensity", i);
    });
    properties.addStacked (QObject::tr ("Intensity"), intensityEdit);
    this->self->registerSecondarySlider (intensityEdit);

    QCheckBox& lockPlaneEdit = ViewUtil::checkBox (QObject::tr ("Lock plane"), params.lockPlane ());
    ViewUtil::connect (lockPlaneEdit, [this, &params](bool v) {
      params.lockPlane (v);
      params.resetLockedPlane ();
      this->self->cache ().set ("lock-plane", v);
    });
    properties.add (lockPlaneEdit);
  }

  void runSetupToolTip (ViewToolTip& toolTip)
  {
    this->self->addDefaultToolTip (toolTip, false);
    this->self->addSecSliderWheelToolTip (toolTip, QObject::tr ("Change intensity"),
                                          QObject::tr ("Drag to change intensity"));
  }

  bool runSculptPointingEvent (const ViewPointingEvent& e)
  {
    auto& params = this->self->brush ().parameters<SBFlattenParameters> ();

    if (params.lockPlane ())
    {
      if (e.pressEvent ())
      {
        DynamicMeshIntersection intersection;
        if (this->self->intersectsScene (e, intersection))
        {
          params.lockedPlane (PrimPlane (intersection.position (), intersection.normal ()));
          this->movement.position (intersection.position ());
          this->movement.freePlaneConstraint (intersection.normal ());
        }
        else
        {
          params.resetLockedPlane ();
        }
      }
      return this->self->grablikeStroke (e, this->movement);
    }
    else
    {
      return this->self->drawlikeStroke (e, false);
    }
  }
};

DELEGATE_TOOL_SCULPT (ToolSculptFlatten)

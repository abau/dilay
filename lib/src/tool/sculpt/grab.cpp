/* This file is part of Dilay
 * Copyright © 2015-2017 Alexander Bau
 * Use and redistribute under the terms of the GNU General Public License
 */
#include <QCheckBox>
#include "cache.hpp"
#include "state.hpp"
#include "tool/sculpt/util/brush.hpp"
#include "tool/util/movement.hpp"
#include "tools.hpp"
#include "view/pointing-event.hpp"
#include "view/two-column-grid.hpp"
#include "view/util.hpp"

struct ToolSculptGrab::Impl
{
  ToolSculptGrab*  self;
  ToolUtilMovement movement;

  Impl (ToolSculptGrab* s)
    : self (s)
    , movement (this->self->state ().camera (), false)
  {
  }

  void runSetupBrush (SculptBrush& brush)
  {
    auto& params = brush.initParameters<SBGrablikeParameters> ();

    params.discardBack (this->self->cache ().get<bool> ("discard-back", false));
  }

  void runSetupCursor (ViewCursor&) {}

  void runSetupProperties (ViewTwoColumnGrid& properties)
  {
    auto& params = this->self->brush ().parameters<SBGrablikeParameters> ();

    this->self->addMoveOnPrimaryPlaneProperties (this->movement);

    QCheckBox& discardEdit =
      ViewUtil::checkBox (QObject::tr ("Discard backfaces"), params.discardBack ());
    ViewUtil::connect (discardEdit, [this, &params](bool d) {
      params.discardBack (d);
      this->self->cache ().set ("discard-back", d);
    });
    properties.add (discardEdit);
  }

  void runSetupToolTip (ViewToolTip& toolTip) { this->self->addDefaultToolTip (toolTip, false); }

  bool runSculptPointingEvent (const ViewPointingEvent& e)
  {
    return this->self->grablikeStroke (e, this->movement);
  }
};

DELEGATE_TOOL_SCULPT (ToolSculptGrab, "sculpt/grab")

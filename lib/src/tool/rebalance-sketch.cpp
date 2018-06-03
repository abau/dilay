/* This file is part of Dilay
 * Copyright © 2015-2018 Alexander Bau
 * Use and redistribute under the terms of the GNU General Public License
 */
#include <QObject>
#include "render-mode.hpp"
#include "scene.hpp"
#include "sketch/mesh.hpp"
#include "sketch/node-intersection.hpp"
#include "state.hpp"
#include "tools.hpp"
#include "view/pointing-event.hpp"
#include "view/tool-tip.hpp"

struct ToolRebalanceSketch::Impl
{
  ToolRebalanceSketch* self;
  const bool           renderWireframe;

  Impl (ToolRebalanceSketch* s)
    : self (s)
    , renderWireframe (s->state ().scene ().renderWireframe ())
  {
  }

  ToolResponse runInitialize ()
  {
    this->self->state ().scene ().renderWireframe (true);

    ViewToolTip toolTip;
    toolTip.add (ViewInputEvent::MouseLeft, QObject::tr ("Set new root"));
    this->self->state ().setToolTip (&toolTip);

    return ToolResponse::None;
  }

  ToolResponse runReleaseEvent (const ViewPointingEvent& e)
  {
    if (e.leftButton ())
    {
      SketchNodeIntersection intersection;
      if (this->self->intersectsScene (e, intersection))
      {
        this->self->snapshotSketchMeshes ();
        intersection.mesh ().rebalance (intersection.node ());
        return ToolResponse::Redraw;
      }
    }
    return ToolResponse::None;
  }

  ToolResponse runCommit ()
  {
    this->self->state ().scene ().renderWireframe (this->renderWireframe);
    return ToolResponse::Redraw;
  }
};

DELEGATE_TOOL (ToolRebalanceSketch)
DELEGATE_TOOL_RUN_RELEASE_EVENT (ToolRebalanceSketch)
DELEGATE_TOOL_RUN_COMMIT (ToolRebalanceSketch)

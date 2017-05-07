/* This file is part of Dilay
 * Copyright © 2015-2017 Alexander Bau
 * Use and redistribute under the terms of the GNU General Public License
 */
#include <QObject>
#include "dynamic/mesh-intersection.hpp"
#include "scene.hpp"
#include "state.hpp"
#include "tools.hpp"
#include "view/pointing-event.hpp"
#include "view/tool-tip.hpp"

struct ToolDeleteMesh::Impl
{
  ToolDeleteMesh* self;

  Impl (ToolDeleteMesh* s)
    : self (s)
  {
  }

  ToolResponse runInitialize ()
  {
    ViewToolTip toolTip;
    toolTip.add (ViewToolTip::Event::MouseLeft, QObject::tr ("Delete selection"));
    this->self->showToolTip (toolTip);

    return ToolResponse::None;
  }

  ToolResponse runReleaseEvent (const ViewPointingEvent& e)
  {
    if (e.primaryButton ())
    {
      DynamicMeshIntersection intersection;
      if (this->self->intersectsScene (e, intersection))
      {
        Scene& scene = this->self->state ().scene ();
        this->self->snapshotDynamicMeshes ();
        scene.deleteMesh (intersection.mesh ());
        return ToolResponse::Redraw;
      }
    }
    return ToolResponse::None;
  }
};

DELEGATE_TOOL (ToolDeleteMesh)
DELEGATE_TOOL_RUN_RELEASE_EVENT (ToolDeleteMesh)

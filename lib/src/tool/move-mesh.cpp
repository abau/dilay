/* This file is part of Dilay
 * Copyright © 2015-2017 Alexander Bau
 * Use and redistribute under the terms of the GNU General Public License
 */
#include <glm/glm.hpp>
#include "cache.hpp"
#include "dynamic/mesh-intersection.hpp"
#include "dynamic/mesh.hpp"
#include "state.hpp"
#include "tool/util/movement.hpp"
#include "tools.hpp"
#include "view/pointing-event.hpp"
#include "view/tool-tip.hpp"
#include "view/two-column-grid.hpp"

struct ToolMoveMesh::Impl
{
  ToolMoveMesh*    self;
  DynamicMesh*     mesh;
  ToolUtilMovement movement;

  Impl (ToolMoveMesh* s)
    : self (s)
    , mesh (nullptr)
    , movement (s->state ().camera (),
                s->cache ().getFrom<MovementFixedConstraint> ("constraint",
                                                              MovementFixedConstraint::CameraPlane))
  {
  }

  ToolResponse runInitialize ()
  {
    this->setupProperties ();
    this->setupToolTip ();

    return ToolResponse::None;
  }

  void setupProperties ()
  {
    this->movement.addFixedProperties (this->self->properties (), [this]() {
      this->setupToolTip ();
      this->self->cache ().set ("constraint", this->movement.fixedConstraint ());
    });
  }

  void setupToolTip ()
  {
    ViewToolTip toolTip;
    toolTip.add (ViewToolTip::Event::MouseLeft, QObject::tr ("Drag to move"));

    if (this->movement.fixedConstraint () != MovementFixedConstraint::CameraPlane)
    {
      toolTip.add (ViewToolTip::Event::MouseLeft, ViewToolTip::Modifier::Shift,
                   QObject::tr ("Drag to move orthogonally"));
    }
    this->self->showToolTip (toolTip);
  }

  ToolResponse runMoveEvent (const ViewPointingEvent& e)
  {
    if (e.leftButton () && this->mesh && this->movement.move (e, true))
    {
      this->mesh->translate (this->movement.delta ());
      return ToolResponse::Redraw;
    }
    else
    {
      return ToolResponse::None;
    }
  }

  ToolResponse runPressEvent (const ViewPointingEvent& e)
  {
    if (e.leftButton ())
    {
      DynamicMeshIntersection intersection;
      if (this->self->intersectsScene (e, intersection))
      {
        this->mesh = &intersection.mesh ();
        this->movement.resetPosition (intersection.position ());

        this->self->snapshotDynamicMeshes ();
      }
    }
    return ToolResponse::None;
  }

  ToolResponse runReleaseEvent (const ViewPointingEvent& e)
  {
    if (e.leftButton () && this->mesh)
    {
      this->mesh->normalize ();
      this->mesh->bufferData ();
      this->mesh = nullptr;
    }
    return ToolResponse::None;
  }
};

DELEGATE_TOOL (ToolMoveMesh)
DELEGATE_TOOL_RUN_MOVE_EVENT (ToolMoveMesh)
DELEGATE_TOOL_RUN_PRESS_EVENT (ToolMoveMesh)
DELEGATE_TOOL_RUN_RELEASE_EVENT (ToolMoveMesh)

/* This file is part of Dilay
 * Copyright © 2015-2018 Alexander Bau
 * Use and redistribute under the terms of the GNU General Public License
 */
#include <glm/glm.hpp>
#include "cache.hpp"
#include "dynamic/mesh-intersection.hpp"
#include "dynamic/mesh.hpp"
#include "mesh.hpp"
#include "primitive/aabox.hpp"
#include "state.hpp"
#include "tool/util/rotation.hpp"
#include "tools.hpp"
#include "view/pointing-event.hpp"
#include "view/tool-tip.hpp"
#include "view/two-column-grid.hpp"
#include "view/util.hpp"

namespace
{
  enum class RotationOrigin
  {
    Intersection,
    Center,
    Origin
  };
}

struct ToolRotateMesh::Impl
{
  ToolRotateMesh*  self;
  DynamicMesh*     mesh;
  RotationOrigin   origin;
  ToolUtilRotation rotation;

  Impl (ToolRotateMesh* s)
    : self (s)
    , mesh (nullptr)
    , origin (RotationOrigin (s->cache ().get<int> ("origin", int(RotationOrigin::Center))))
    , rotation (s->state ().camera ())
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
    ViewTwoColumnGrid& properties = this->self->properties ();

    QButtonGroup& originEdit = ViewUtil::buttonGroup (
      {QObject::tr ("Intersection"), QObject::tr ("Center"), QObject::tr ("Origin")});
    ViewUtil::connect (originEdit, int(this->origin), [this](int id) {
      this->origin = RotationOrigin (id);
      this->self->cache ().set ("origin", id);
    });
    properties.add (originEdit);
  }

  void setupToolTip ()
  {
    ViewToolTip toolTip;
    toolTip.add (ViewInput::Event::MouseLeft, QObject::tr ("Drag to rotate"));
    this->self->showToolTip (toolTip);
  }

  ToolResponse runMoveEvent (const ViewPointingEvent& e)
  {
    if (e.leftButton () && this->mesh && this->rotation.rotate (e))
    {
      this->mesh->rotate (this->rotation.matrix ());
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
        switch (this->origin)
        {
          case RotationOrigin::Intersection:
            this->rotation.reset (intersection.position ());
            break;
          case RotationOrigin::Center:
            this->rotation.reset (this->mesh->mesh ().bounds ().center ());
            break;
          case RotationOrigin::Origin:
            this->rotation.reset (glm::vec3 (0.0f));
            break;
        }
        this->self->snapshotDynamicMeshes ();
      }
    }
    return ToolResponse::None;
  }

  ToolResponse runReleaseEvent (const ViewPointingEvent& e)
  {
    if (e.leftButton ())
    {
      return this->runCommit ();
    }
    return ToolResponse::None;
  }

  ToolResponse runCommit ()
  {
    if (this->mesh)
    {
      this->mesh->normalize ();
      this->mesh->bufferData ();
      this->mesh = nullptr;
    }
    return ToolResponse::None;
  }
};

DELEGATE_TOOL (ToolRotateMesh, "rotate-mesh")
DELEGATE_TOOL_RUN_MOVE_EVENT (ToolRotateMesh)
DELEGATE_TOOL_RUN_PRESS_EVENT (ToolRotateMesh)
DELEGATE_TOOL_RUN_RELEASE_EVENT (ToolRotateMesh)
DELEGATE_TOOL_RUN_COMMIT (ToolRotateMesh)

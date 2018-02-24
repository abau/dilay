/* This file is part of Dilay
 * Copyright © 2015-2018 Alexander Bau
 * Use and redistribute under the terms of the GNU General Public License
 */
#include <QPainter>
#include <QSlider>
#include <QWheelEvent>
#include "cache.hpp"
#include "camera.hpp"
#include "color.hpp"
#include "config.hpp"
#include "dynamic/mesh-intersection.hpp"
#include "dynamic/mesh.hpp"
#include "isosurface-extraction.hpp"
#include "mesh.hpp"
#include "primitive/aabox.hpp"
#include "primitive/ray.hpp"
#include "scene.hpp"
#include "state.hpp"
#include "tools.hpp"
#include "view/main-window.hpp"
#include "view/pointing-event.hpp"
#include "view/tool-tip.hpp"
#include "view/two-column-grid.hpp"
#include "view/util.hpp"

struct ToolRemesh::Impl
{
  ToolRemesh* self;

  Impl (ToolRemesh* s)
    : self (s)
  {
  }

  void setupProperties () {}

  void setupToolTip ()
  {
    ViewToolTip toolTip;
    toolTip.add (ViewInput::Event::MouseLeft, QObject::tr ("Remesh selection"));
    this->self->showToolTip (toolTip);
  }

  ToolResponse runInitialize ()
  {
    this->setupProperties ();
    this->setupToolTip ();

    return ToolResponse::None;
  }

  void remesh (DynamicMesh& mesh)
  {
    const float resolution = 0.05f;

    glm::vec3 min, max;
    mesh.mesh ().minMax (min, max);

    const IsosurfaceExtraction::IntersectionCallback getIntersection =
      [&mesh](const PrimRay& ray, Intersection& intersection) {
        return mesh.intersects (ray, intersection, true);
      };

    const IsosurfaceExtraction::InsideCallback isInside = [&mesh](const glm::vec3& pos) {
      const PrimRay ray (pos, glm::vec3 (1.0f, 0.0f, 0.0f));
      Intersection  intersection;
      if (mesh.intersects (ray, intersection, true))
      {
        return glm::dot (intersection.normal (), ray.direction ()) > 0.0f;
      }
      else
      {
        return false;
      }
    };

    const IsosurfaceExtraction::DistanceCallback getDistance = [&mesh](const glm::vec3& pos) {
      return mesh.unsignedDistance (pos);
    };

    const PrimAABox bounds (min, max);
    Mesh            newMesh =
      IsosurfaceExtraction::extract (getDistance, getIntersection, isInside, bounds, resolution);

    State& state = this->self->state ();
    state.scene ().deleteMesh (mesh);
    state.scene ().newDynamicMesh (state.config (), newMesh);
  }

  ToolResponse runReleaseEvent (const ViewPointingEvent& e)
  {
    if (e.leftButton () == false)
    {
      return ToolResponse::None;
    }
    else
    {
      DynamicMeshIntersection intersection;
      if (this->self->intersectsScene (e.position (), intersection))
      {
        this->self->snapshotDynamicMeshes ();
        this->remesh (intersection.mesh ());
        return ToolResponse::Redraw;
      }
      else
      {
        return ToolResponse::None;
      }
    }
  }

  ToolResponse runCommit () { return ToolResponse::Redraw; }
};

DELEGATE_TOOL (ToolRemesh, "remesh")
DELEGATE_TOOL_RUN_RELEASE_EVENT (ToolRemesh)
DELEGATE_TOOL_RUN_COMMIT (ToolRemesh)

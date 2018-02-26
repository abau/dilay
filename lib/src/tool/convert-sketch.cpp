/* This file is part of Dilay
 * Copyright © 2015-2018 Alexander Bau
 * Use and redistribute under the terms of the GNU General Public License
 */
#include <QCheckBox>
#include "cache.hpp"
#include "distance.hpp"
#include "dynamic/mesh.hpp"
#include "isosurface-extraction.hpp"
#include "mesh.hpp"
#include "primitive/aabox.hpp"
#include "primitive/cone-sphere.hpp"
#include "scene.hpp"
#include "sketch/mesh-intersection.hpp"
#include "sketch/mesh.hpp"
#include "sketch/path.hpp"
#include "state.hpp"
#include "tool/sculpt/util/action.hpp"
#include "tools.hpp"
#include "view/double-slider.hpp"
#include "view/pointing-event.hpp"
#include "view/tool-tip.hpp"
#include "view/two-column-grid.hpp"
#include "view/util.hpp"

namespace
{
  glm::vec3 computeCenter (const SketchMesh& mesh)
  {
    if (mesh.tree ().hasRoot ())
    {
      return mesh.tree ().root ().data ().center ();
    }
    else
    {
      glm::vec3 min, max;
      mesh.minMax (min, max);
      return ((min + max) * 0.5f);
    }
  }
}

struct ToolConvertSketch::Impl
{
  ToolConvertSketch* self;
  const float        minResolution;
  const float        maxResolution;
  float              resolution;
  bool               moveToCenter;

  Impl (ToolConvertSketch* s)
    : self (s)
    , minResolution (0.01f)
    , maxResolution (0.1f)
    , resolution (s->cache ().get<float> ("resolution", 0.06))
    , moveToCenter (s->cache ().get<bool> ("move-to-center", true))
  {
  }

  ToolResponse runInitialize ()
  {
    this->setupProperties ();
    this->setupToolTip ();

    return ToolResponse::Redraw;
  }

  void setupProperties ()
  {
    ViewTwoColumnGrid& properties = this->self->properties ();

    ViewDoubleSlider& resolutionEdit =
      ViewUtil::slider (2, this->minResolution, this->resolution, this->maxResolution);
    ViewUtil::connect (resolutionEdit, [this](float r) {
      this->resolution = r;
      this->self->cache ().set ("resolution", r);
    });
    properties.addStacked (QObject::tr ("Resolution"), resolutionEdit);

    QCheckBox& moveToCenterEdit =
      ViewUtil::checkBox (QObject::tr ("Move to center"), this->moveToCenter);
    ViewUtil::connect (moveToCenterEdit, [this](bool m) {
      this->moveToCenter = m;
      this->self->cache ().set ("move-to-center", m);
    });
    properties.add (moveToCenterEdit);
  }

  void setupToolTip ()
  {
    ViewToolTip toolTip;
    toolTip.add (ViewInput::Event::MouseLeft, QObject::tr ("Convert selection"));
    this->self->showToolTip (toolTip);
  }

  DynamicMesh& convert (SketchMesh& sketch)
  {
    const float resolution = this->maxResolution + this->minResolution - this->resolution;

    glm::vec3 min, max;
    sketch.minMax (min, max);

    const IsosurfaceExtraction::DistanceCallback getDistance = [&sketch](const glm::vec3& pos) {
      float distance = Util::maxFloat ();

      if (sketch.tree ().hasRoot ())
      {
        sketch.tree ().root ().forEachConstNode ([&pos, &distance](const SketchNode& node) {
          const float d =
            node.parent ()
              ? Distance::distance (PrimConeSphere (node.data (), node.parent ()->data ()), pos)
              : Distance::distance (node.data (), pos);

          distance = glm::min (distance, d);
        });
      }
      for (const SketchPath& p : sketch.paths ())
      {
        for (const PrimSphere& s : p.spheres ())
        {
          distance = glm::min (distance, Distance::distance (s, pos));
        }
      }
      return distance;
    };

    sketch.optimizePaths ();
    Mesh mesh = IsosurfaceExtraction::extract (getDistance, PrimAABox (min, max), resolution);

    State& state = this->self->state ();
    return state.scene ().newDynamicMesh (state.config (), mesh);
  }

  ToolResponse runReleaseEvent (const ViewPointingEvent& e)
  {
    if (e.leftButton ())
    {
      SketchMeshIntersection intersection;
      if (this->self->intersectsScene (e, intersection))
      {
        SketchMesh& sMesh = intersection.mesh ();
        glm::vec3   center = computeCenter (sMesh);

        this->self->snapshotAll ();

        DynamicMesh& dMesh = this->convert (sMesh);

        if (this->moveToCenter)
        {
          dMesh.translate (-center);
          dMesh.normalize ();
          dMesh.bufferData ();
        }
        ToolSculptAction::smoothMesh (dMesh);
        this->self->state ().scene ().deleteMesh (sMesh);
        return ToolResponse::Redraw;
      }
    }
    return ToolResponse::None;
  }
};

DELEGATE_TOOL (ToolConvertSketch, "convert-sketch")
DELEGATE_TOOL_RUN_RELEASE_EVENT (ToolConvertSketch)

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
#include "view/pointing-event.hpp"
#include "view/resolution-slider.hpp"
#include "view/tool-tip.hpp"
#include "view/two-column-grid.hpp"
#include "view/util.hpp"

struct ToolConvertSketch::Impl
{
  ToolConvertSketch* self;
  float              resolution;

  Impl (ToolConvertSketch* s)
    : self (s)
    , resolution (s->cache ().get<float> ("resolution", 0.06))
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

    ViewResolutionSlider& resolutionEdit =
      ViewUtil::resolutionSlider (0.01f, this->resolution, 0.1f);
    ViewUtil::connect (resolutionEdit, [this](float r) {
      this->resolution = r;
      this->self->cache ().set ("resolution", r);
    });
    properties.addStacked (QObject::tr ("Resolution"), resolutionEdit);
  }

  void setupToolTip ()
  {
    ViewToolTip toolTip;
    toolTip.add (ViewInputEvent::MouseLeft, QObject::tr ("Convert selection"));
    this->self->state ().setToolTip (&toolTip);
  }

  DynamicMesh& convert (SketchMesh& sketch)
  {
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
    DynamicMesh mesh;
    IsosurfaceExtraction::extract (getDistance, PrimAABox (min, max), this->resolution, mesh);

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

        this->self->snapshotAll ();

        ToolSculptAction::smoothMesh (this->convert (sMesh));
        this->self->state ().scene ().deleteMesh (sMesh);
        return ToolResponse::Redraw;
      }
    }
    return ToolResponse::None;
  }
};

DELEGATE_TOOL (ToolConvertSketch)
DELEGATE_TOOL_RUN_RELEASE_EVENT (ToolConvertSketch)

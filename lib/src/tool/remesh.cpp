/* This file is part of Dilay
 * Copyright © 2015-2018 Alexander Bau
 * Use and redistribute under the terms of the GNU General Public License
 */
#include <QPainter>
#include "cache.hpp"
#include "color.hpp"
#include "config.hpp"
#include "dynamic/mesh-intersection.hpp"
#include "dynamic/mesh.hpp"
#include "isosurface-extraction.hpp"
#include "maybe.hpp"
#include "mesh.hpp"
#include "primitive/aabox.hpp"
#include "primitive/ray.hpp"
#include "scene.hpp"
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
  enum class Mode
  {
    Normal,
    Union,
    Difference,
    Intersection
  };
}

struct ToolRemesh::Impl
{
  ToolRemesh*       self;
  const float       minResolution;
  const float       maxResolution;
  float             resolution;
  Mode              mode;
  Maybe<glm::ivec2> pressPoint;

  Impl (ToolRemesh* s)
    : self (s)
    , minResolution (0.02f)
    , maxResolution (0.1f)
    , resolution (s->cache ().get<float> ("resolution", 0.06))
    , mode (Mode (s->cache ().get<int> ("mode", int(Mode::Normal))))
  {
  }

  void setupProperties ()
  {
    ViewTwoColumnGrid& properties = this->self->properties ();

    QButtonGroup& modeEdit =
      ViewUtil::buttonGroup ({QObject::tr ("Normal"), QObject::tr ("Union"),
                              QObject::tr ("Difference"), QObject::tr ("Intersection")});
    ViewUtil::connect (modeEdit, int(this->mode), [this](int id) {
      this->mode = Mode (id);
      this->self->cache ().set ("mode", id);
    });
    properties.add (modeEdit);

    ViewDoubleSlider& resolutionEdit =
      ViewUtil::slider (2, this->minResolution, this->resolution, this->maxResolution);
    ViewUtil::connect (resolutionEdit, [this](float r) {
      this->resolution = r;
      this->self->cache ().set ("resolution", r);
    });
    properties.addStacked (QObject::tr ("Resolution"), resolutionEdit);
  }

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

  ToolResponse runMoveEvent (const ViewPointingEvent&)
  {
    return this->mode == Mode::Normal ? ToolResponse::None : ToolResponse::Redraw;
  }

  void remesh (DynamicMesh& mesh)
  {
    glm::vec3 min, max;
    mesh.mesh ().minMax (min, max);

    const IsosurfaceExtraction::IntersectionCallback getIntersection =
      [&mesh](const PrimRay& ray, Intersection& intersection) {
        if (mesh.intersects (ray, intersection, true))
        {
          return IsosurfaceExtraction::Intersection::Sample;
        }
        else
        {
          return IsosurfaceExtraction::Intersection::None;
        }
      };

    const IsosurfaceExtraction::DistanceCallback getDistance = [&mesh](const glm::vec3& pos) {
      return mesh.unsignedDistance (pos);
    };

    const float     resolution = this->maxResolution + this->minResolution - this->resolution;
    const PrimAABox bounds (min, max);
    Mesh newMesh = IsosurfaceExtraction::extract (getDistance, getIntersection, bounds, resolution);

    State& state = this->self->state ();
    state.scene ().deleteMesh (mesh);
    DynamicMesh& dMesh = state.scene ().newDynamicMesh (state.config (), newMesh);
    ToolSculptAction::smoothMesh (dMesh);
  }

  void remesh (DynamicMesh& meshA, DynamicMesh& meshB)
  {
    const IsosurfaceExtraction::IntersectionCallback getCommutativeIntersection =
      [this, &meshA, &meshB](const PrimRay& ray, Intersection& intersection) {

        assert (this->mode == Mode::Union || this->mode == Mode::Intersection);

        Intersection intersectionA, intersectionB;
        meshA.intersects (ray, intersectionA, true);
        meshB.intersects (ray, intersectionB, true);

        Intersection::sort (intersectionA, intersectionB);
        intersection = intersectionA;

        const bool intersectsA = intersectionA.isIntersection ();
        const bool intersectsB = intersectionB.isIntersection ();

        if (intersectsA && intersectsB)
        {
          const bool insideA = glm::dot (ray.direction (), intersectionA.normal ()) > 0.0f;
          const bool insideB = glm::dot (ray.direction (), intersectionB.normal ()) > 0.0f;

          if (insideA && insideB)
          {
            // (B (A o-> A) B)
            // (A (B o-> A) B)
            if (this->mode == Mode::Union)
            {
              return IsosurfaceExtraction::Intersection::Continue;
            }
            else
            {
              assert (this->mode == Mode::Intersection);
              return IsosurfaceExtraction::Intersection::Sample;
            }
          }
          else if (insideA && insideB == false)
          {
            // (A o-> A) (B B)
            if (this->mode == Mode::Union)
            {
              return IsosurfaceExtraction::Intersection::Sample;
            }
            else
            {
              assert (this->mode == Mode::Intersection);
              return IsosurfaceExtraction::Intersection::Continue;
            }
          }
          else if (insideA == false && insideB)
          {
            // (B o-> (A A) B)
            // (B o-> (A B) A)
            if (this->mode == Mode::Union)
            {
              return IsosurfaceExtraction::Intersection::Continue;
            }
            else
            {
              assert (this->mode == Mode::Intersection);
              return IsosurfaceExtraction::Intersection::Sample;
            }
          }
          else if (insideA == false && insideB == false)
          {
            // o-> (A (B B) A)
            // o-> (A (B A) B)
            // o-> (A A) (B B)
            if (this->mode == Mode::Union)
            {
              return IsosurfaceExtraction::Intersection::Sample;
            }
            else
            {
              assert (this->mode == Mode::Intersection);
              return IsosurfaceExtraction::Intersection::Continue;
            }
          }
          DILAY_IMPOSSIBLE
        }
        else if (intersectsA)
        {
          if (this->mode == Mode::Union)
          {
            return IsosurfaceExtraction::Intersection::Sample;
          }
          else
          {
            assert (this->mode == Mode::Intersection);
            return IsosurfaceExtraction::Intersection::Continue;
          }
        }
        else
        {
          assert (intersectsB == false);
          return IsosurfaceExtraction::Intersection::None;
        }
        DILAY_IMPOSSIBLE
      };

    const IsosurfaceExtraction::IntersectionCallback getDifferenceIntersection =
      [this, &meshA, &meshB](const PrimRay& ray, Intersection& intersection) {

        assert (this->mode == Mode::Difference);

        Intersection intersectionA, intersectionB;
        const bool   intersectsA = meshA.intersects (ray, intersectionA, true);
        const bool   intersectsB = meshB.intersects (ray, intersectionB, true);

        if (intersectsA && intersectsB)
        {
          const bool insideA = glm::dot (ray.direction (), intersectionA.normal ()) > 0.0f;
          const bool insideB = glm::dot (ray.direction (), intersectionB.normal ()) > 0.0f;
          const bool aBeforeB = intersectionA.distance () < intersectionB.distance ();

          intersection = aBeforeB ? intersectionA : intersectionB;

          if (insideA && insideB)
          {
            if (aBeforeB)
            {
              // (B (A o-> A) B)
              // (A (B o-> A) B)
              return IsosurfaceExtraction::Intersection::Continue;
            }
            else
            {
              // (A (B o-> B) A)
              // (B (A o-> B) A)
              return IsosurfaceExtraction::Intersection::Sample;
            }
          }
          else if (insideA && insideB == false)
          {
            // (A o-> A) (B B)
            // (A o-> (B B) A)
            // (A o-> (B A) B)
            return IsosurfaceExtraction::Intersection::Sample;
          }
          else if (insideA == false && insideB)
          {
            // (B o-> B) (A A)
            // (B o-> (A A) B)
            // (B o-> (A B) A)
            return IsosurfaceExtraction::Intersection::Continue;
          }
          else if (insideA == false && insideB == false)
          {
            if (aBeforeB)
            {
              // o-> (A (B B) A)
              // o-> (A (B A) B)
              // o-> (A A) (B B)
              return IsosurfaceExtraction::Intersection::Sample;
            }
            else
            {
              // o-> (B (A A) B)
              // o-> (B (A B) A)
              // o-> (B B) (A A)
              return IsosurfaceExtraction::Intersection::Continue;
            }
          }
          DILAY_IMPOSSIBLE
        }
        else if (intersectsA)
        {
          intersection = intersectionA;
          return IsosurfaceExtraction::Intersection::Sample;
        }
        else if (intersectsB)
        {
          intersection = intersectionB;
          return IsosurfaceExtraction::Intersection::Continue;
        }
        else
        {
          return IsosurfaceExtraction::Intersection::None;
        }
        DILAY_IMPOSSIBLE
      };

    const IsosurfaceExtraction::DistanceCallback getDistance = [&meshA,
                                                                &meshB](const glm::vec3& pos) {
      return glm::min (meshA.unsignedDistance (pos), meshB.unsignedDistance (pos));
    };

    glm::vec3 minA, maxA, minB, maxB;
    meshA.mesh ().minMax (minA, maxA);
    meshB.mesh ().minMax (minB, maxB);

    const float     resolution = this->maxResolution + this->minResolution - this->resolution;
    const PrimAABox bounds (glm::min (minA, minB), glm::max (maxA, maxB));
    Mesh            newMesh;
    if (this->mode == Mode::Difference)
    {
      newMesh =
        IsosurfaceExtraction::extract (getDistance, getDifferenceIntersection, bounds, resolution);
    }
    else
    {
      newMesh =
        IsosurfaceExtraction::extract (getDistance, getCommutativeIntersection, bounds, resolution);
    }

    State& state = this->self->state ();
    state.scene ().deleteMesh (meshA);
    state.scene ().deleteMesh (meshB);

    if (newMesh.numIndices () > 0)
    {
      DynamicMesh& dMesh = state.scene ().newDynamicMesh (state.config (), newMesh);
      ToolSculptAction::smoothMesh (dMesh);
    }
  }

  ToolResponse runPressEvent (const ViewPointingEvent& e)
  {
    if (e.leftButton () == false || this->mode == Mode::Normal)
    {
      return ToolResponse::None;
    }
    else
    {
      this->pressPoint = e.position ();
      return ToolResponse::Redraw;
    }
  }

  ToolResponse runReleaseEvent (const ViewPointingEvent& e)
  {
    if (e.leftButton () == false)
    {
      return ToolResponse::None;
    }
    else
    {
      if (this->mode == Mode::Normal)
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
      else if (this->pressPoint)
      {
        DynamicMeshIntersection intersectionA;
        DynamicMeshIntersection intersectionB;
        const bool intersectsA = this->self->intersectsScene (*this->pressPoint, intersectionA);
        const bool intersectsB = this->self->intersectsScene (e.position (), intersectionB);

        this->pressPoint.reset ();

        if (intersectsA && intersectsB)
        {
          this->self->snapshotDynamicMeshes ();
          if (&intersectionA.mesh () == &intersectionB.mesh ())
          {
            this->remesh (intersectionA.mesh ());
          }
          else
          {
            this->remesh (intersectionA.mesh (), intersectionB.mesh ());
          }
          return ToolResponse::Redraw;
        }
        else
        {
          return ToolResponse::None;
        }
      }
      else
      {
        return ToolResponse::None;
      }
    }
  }

  void runPaint (QPainter& painter) const
  {
    if (this->mode != Mode::Normal && this->pressPoint)
    {
      const QPoint cursorPos (ViewUtil::toQPoint (this->self->cursorPosition ()));

      QPen pen (this->self->config ().get<Color> ("editor/on-screen-color").qColor ());
      pen.setCapStyle (Qt::FlatCap);
      pen.setWidth (2);

      painter.setPen (pen);
      painter.drawLine (ViewUtil::toQPoint (*this->pressPoint), cursorPos);
    }
  }

  ToolResponse runCommit () { return ToolResponse::Redraw; }
};

DELEGATE_TOOL (ToolRemesh, "remesh")
DELEGATE_TOOL_RUN_MOVE_EVENT (ToolRemesh)
DELEGATE_TOOL_RUN_PRESS_EVENT (ToolRemesh)
DELEGATE_TOOL_RUN_RELEASE_EVENT (ToolRemesh)
DELEGATE_TOOL_RUN_PAINT (ToolRemesh)
DELEGATE_TOOL_RUN_COMMIT (ToolRemesh)

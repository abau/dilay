/* This file is part of Dilay
 * Copyright © 2015-2018 Alexander Bau
 * Use and redistribute under the terms of the GNU General Public License
 */
#include <QWheelEvent>
#include <glm/gtc/constants.hpp>
#include "camera.hpp"
#include "config.hpp"
#include "dimension.hpp"
#include "intersection.hpp"
#include "primitive/ray.hpp"
#include "scene.hpp"
#include "state.hpp"
#include "tool/move-camera.hpp"
#include "util.hpp"
#include "view/floor-plane.hpp"
#include "view/gl-widget.hpp"
#include "view/main-window.hpp"
#include "view/pointing-event.hpp"

struct ToolMoveCamera::Impl
{
  glm::ivec2 oldPos;
  float      rotationFactor;
  float      movementFactor;
  float      zoomInFactor;
  float      zoomInMouseWheelFactor;

  Impl (const Config& config) { this->runFromConfig (config); }

  void snap (State& state, bool cycleBackwards)
  {
    Camera& cam = state.camera ();

    auto snapTo = [&cam](Dimension d, bool forcePositive) {
      const float gazeLength = glm::length (cam.toEyePoint ());
      const float side =
        (forcePositive || cam.toEyePoint ()[DimensionUtil::index (d)] > 0.0f) ? 1.0f : -1.0f;

      const glm::vec3 toEyePoint = side * gazeLength * DimensionUtil::vector (d);

      cam.set (cam.gazePoint (), toEyePoint);
    };

    auto isSnapped = [&cam](Dimension d) -> bool {
      const float dot =
        glm::abs (glm::dot (glm::normalize (cam.toEyePoint ()), DimensionUtil::vector (d)));
      return dot > 1.0f - Util::epsilon ();
    };

    if (cycleBackwards)
    {
      if (isSnapped (Dimension::X))
      {
        snapTo (Dimension::Z, true);
      }
      else if (isSnapped (Dimension::Y))
      {
        snapTo (Dimension::X, true);
      }
      else if (isSnapped (Dimension::Z))
      {
        snapTo (Dimension::Y, true);
      }
      else
      {
        snapTo (cam.primaryDimension (), false);
      }
    }
    else
    {
      if (isSnapped (Dimension::X))
      {
        snapTo (Dimension::Y, true);
      }
      else if (isSnapped (Dimension::Y))
      {
        snapTo (Dimension::Z, true);
      }
      else if (isSnapped (Dimension::Z))
      {
        snapTo (Dimension::X, true);
      }
      else
      {
        snapTo (cam.primaryDimension (), false);
      }
    }
    state.mainWindow ().glWidget ().update ();
  }

  void resetGazePoint (State& state)
  {
    Camera& cam = state.camera ();

    cam.set (glm::vec3 (0.0f), cam.position ());
    state.mainWindow ().glWidget ().update ();
  }

  void moveEvent (State& state, const ViewPointingEvent& event)
  {
    if (event.middleButton ())
    {
      Camera&          cam = state.camera ();
      const glm::vec2& resolution = glm::vec2 (cam.resolution ());
      const glm::ivec2 newPos = event.position ();
      const glm::vec2  delta = glm::vec2 (newPos) - glm::vec2 (this->oldPos);

      if (event.modifiers () == Qt::NoModifier)
      {
        if (delta.x != 0.0f)
        {
          cam.verticalRotation (2.0f * glm::pi<float> () * this->rotationFactor * -delta.x /
                                resolution.x);
        }
        if (delta.y != 0.0f)
        {
          cam.horizontalRotation (2.0f * glm::pi<float> () * this->rotationFactor * -delta.y /
                                  resolution.y);
        }
      }
      else if (event.modifiers () == Qt::ShiftModifier)
      {
        cam.setGaze (cam.gazePoint () + (this->movementFactor * -delta.x * cam.right ()) +
                     (this->movementFactor * delta.y * cam.up ()));
      }
      else if (event.modifiers () == Qt::ControlModifier)
      {
        if (delta.y < 0.0f)
        {
          state.camera ().stepAlongGaze (this->zoomInFactor);
        }
        else if (delta.y > 0.0f)
        {
          state.camera ().stepAlongGaze (1.0f / this->zoomInFactor);
        }
        state.mainWindow ().glWidget ().floorPlane ().update (state.camera ());
      }
      this->oldPos = newPos;
      state.mainWindow ().glWidget ().update ();
    }
  }

  void pressEvent (State& state, const ViewPointingEvent& event)
  {
    if (event.middleButton ())
    {
      this->oldPos = event.position ();

      if (event.modifiers () == Qt::AltModifier)
      {
        Camera&      cam = state.camera ();
        Intersection intersection;
        if (state.scene ().intersects (cam.ray (event.position ()), intersection))
        {
          cam.set (intersection.position (), cam.position () - intersection.position ());
          state.mainWindow ().glWidget ().update ();
        }
      }
    }
  }

  void wheelEvent (State& state, const QWheelEvent& event)
  {
    if (event.orientation () == Qt::Vertical)
    {
      if (event.delta () > 0)
      {
        state.camera ().stepAlongGaze (this->zoomInMouseWheelFactor);
      }
      else if (event.delta () < 0)
      {
        state.camera ().stepAlongGaze (1.0f / this->zoomInMouseWheelFactor);
      }
      state.mainWindow ().glWidget ().floorPlane ().update (state.camera ());
      state.mainWindow ().glWidget ().update ();
    }
  }

  void runFromConfig (const Config& config)
  {
    this->rotationFactor = config.get<float> ("editor/camera/rotation-factor");
    this->movementFactor = config.get<float> ("editor/camera/movement-factor");
    this->zoomInFactor = config.get<float> ("editor/camera/zoom-in-factor");
    this->zoomInMouseWheelFactor = config.get<float> ("editor/camera/zoom-in-mouse-wheel-factor");
  }
};

DELEGATE1_BIG3 (ToolMoveCamera, const Config&)
DELEGATE2 (void, ToolMoveCamera, snap, State&, bool)
DELEGATE1 (void, ToolMoveCamera, resetGazePoint, State&)
DELEGATE2 (void, ToolMoveCamera, moveEvent, State&, const ViewPointingEvent&)
DELEGATE2 (void, ToolMoveCamera, pressEvent, State&, const ViewPointingEvent&)
DELEGATE2 (void, ToolMoveCamera, wheelEvent, State&, const QWheelEvent&)
DELEGATE1 (void, ToolMoveCamera, runFromConfig, const Config&)

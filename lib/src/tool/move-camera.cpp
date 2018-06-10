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
#include "state.hpp"
#include "tools.hpp"
#include "view/floor-plane.hpp"
#include "view/gl-widget.hpp"
#include "view/main-window.hpp"
#include "view/pointing-event.hpp"
#include "view/shortcut.hpp"
#include "view/tool-tip.hpp"

struct ToolMoveCamera::Impl
{
  ToolMoveCamera* self;
  bool            isImmediateTool;
  glm::ivec2      oldPos;
  float           rotationFactor;
  float           movementFactor;
  float           zoomInMouseWheelFactor;

  Impl (ToolMoveCamera* s, bool i)
    : self (s)
    , isImmediateTool (i)
    , oldPos (s->state ().mainWindow ().glWidget ().cursorPosition ())
  {
  }

  Impl (ToolMoveCamera* s)
    : Impl (s, false)
  {
  }

  ToolResponse runInitialize ()
  {
    if (this->isImmediateTool == false)
    {
      this->setupToolTip ();
    }
    return ToolResponse::None;
  }

  void setupToolTip ()
  {
    ViewToolTip toolTip;
    toolTip.add (ViewInputEvent::MouseLeft, QObject::tr ("Drag to rotate"));
    toolTip.add (ViewInputEvent::MouseLeft, ViewInputModifier::Shift, QObject::tr ("Drag to move"));
    toolTip.add (ViewInputEvent::MouseLeft, ViewInputModifier::Ctrl, QObject::tr ("Drag to zoom"));
    toolTip.add (ViewInputEvent::MouseLeft, ViewInputModifier::Alt, QObject::tr ("Gaze"));
    toolTip.add (ViewInputEvent::C, ViewInputModifier::Shift, QObject::tr ("Snap camera"));
    toolTip.add (ViewInputEvent::C, ViewInputModifier::Alt, QObject::tr ("Reset gaze point"));

    const auto exitShortcut = ViewShortcut (ViewInputEvent::Space, QObject::tr ("Exit camera mode"),
                                            [this]() { this->self->state ().setPreviousTool (); });

    this->self->state ().setToolTip (&toolTip, {exitShortcut});
  }

  bool mouseButton (const ViewPointingEvent& e)
  {
    return this->isImmediateTool ? e.middleButton () : e.leftButton ();
  }

  ToolResponse runMoveEvent (const ViewPointingEvent& e)
  {
    if (this->mouseButton (e))
    {
      Camera&          cam = this->self->state ().camera ();
      const glm::vec2& resolution = glm::vec2 (cam.resolution ());
      const glm::vec2  delta = glm::vec2 (e.position ()) - glm::vec2 (this->oldPos);

      if (e.modifiers () == Qt::NoModifier)
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
      else if (e.modifiers () == Qt::ShiftModifier)
      {
        cam.setGaze (cam.gazePoint () + (this->movementFactor * delta.x * cam.right ()) +
                     (this->movementFactor * -delta.y * cam.realUp ()));
      }
      else if (e.modifiers () == Qt::ControlModifier)
      {
        const float y = glm::clamp (delta.y, -50.0f, 50.0f);

        if (y < 0.0f)
        {
          cam.stepAlongGaze (1.0f - (-y / 100.0f));
        }
        else if (y > 0.0f)
        {
          cam.stepAlongGaze (1.0f / (1.0f - (y / 100.0f)));
        }
        this->self->state ().mainWindow ().glWidget ().floorPlane ().update (cam);
      }
      this->oldPos = e.position ();
      return ToolResponse::Redraw;
    }
    else
    {
      this->oldPos = e.position ();
      return ToolResponse::None;
    }
  }

  ToolResponse runPressEvent (const ViewPointingEvent& e)
  {
    if (this->mouseButton (e) && e.modifiers () == Qt::AltModifier)
    {
      Intersection intersection;
      if (this->self->intersectsScene (e.position (), intersection))
      {
        Camera& camera = this->self->state ().camera ();
        camera.set (intersection.position (), camera.position () - intersection.position ());
        return ToolResponse::Redraw;
      }
    }
    return ToolResponse::None;
  }

  void runFromConfig ()
  {
    const Config& config = this->self->config ();

    this->rotationFactor = config.get<float> ("editor/camera/rotation-factor");
    this->movementFactor = config.get<float> ("editor/camera/movement-factor");
    this->zoomInMouseWheelFactor = config.get<float> ("editor/camera/zoom-in-mouse-wheel-factor");
  }

  ToolResponse wheelEvent (const QWheelEvent& e)
  {
    if (this->isImmediateTool && e.modifiers () == Qt::NoModifier &&
        e.orientation () == Qt::Vertical)
    {
      Camera& camera = this->self->state ().camera ();

      if (e.delta () > 0)
      {
        camera.stepAlongGaze (this->zoomInMouseWheelFactor);
      }
      else if (e.delta () < 0)
      {
        camera.stepAlongGaze (1.0f / this->zoomInMouseWheelFactor);
      }
      this->self->state ().mainWindow ().glWidget ().floorPlane ().update (camera);
      return ToolResponse::Redraw;
    }
    return ToolResponse::None;
  }

  void snap ()
  {
    Camera& cam = this->self->state ().camera ();

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
    this->self->state ().mainWindow ().glWidget ().update ();
  }

  void resetGazePoint ()
  {
    Camera& cam = this->self->state ().camera ();

    cam.set (glm::vec3 (0.0f), cam.position ());
    this->self->state ().mainWindow ().glWidget ().update ();
  }
};

DELEGATE_CONSTRUCTOR_BASE (ToolMoveCamera, (State & s, bool i), (this, i), Tool,
                           (s, "ToolMoveCamera"))
DELEGATE_TOOL (ToolMoveCamera)
DELEGATE_TOOL_RUN_MOVE_EVENT (ToolMoveCamera)
DELEGATE_TOOL_RUN_PRESS_EVENT (ToolMoveCamera)
DELEGATE_TOOL_RUN_FROM_CONFIG (ToolMoveCamera)
DELEGATE1 (ToolResponse, ToolMoveCamera, wheelEvent, const QWheelEvent&)
DELEGATE (void, ToolMoveCamera, snap)
DELEGATE (void, ToolMoveCamera, resetGazePoint)

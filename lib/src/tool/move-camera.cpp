#include <QWheelEvent>
#include <glm/glm.hpp>
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
#include "view/gl-widget.hpp"
#include "view/main-window.hpp"
#include "view/util.hpp"
#include "winged/face-intersection.hpp"

struct ToolMoveCamera::Impl {
  glm::ivec2 oldPos;
  float      rotationFactor;
  float      panningFactor;
  float      zoomInFactor;

  Impl (const Config& config) {
    this->runFromConfig (config);
  }

  void keyPressEvent (State& state, const QKeyEvent& event) {
    const int                   key = event.key ();
    const Qt::KeyboardModifiers mod = event.modifiers ();
          Camera&               cam = state.camera ();

    auto snapCamera = [&cam] (Dimension d, bool forcePositive) {
      const float gazeLength = glm::length (cam.toEyePoint ());
      const float side       = (forcePositive || cam.toEyePoint ()[DimensionUtil::index (d)] > 0.0f)
                             ?  1.0f
                             : -1.0f;

      const glm::vec3 toEyePoint = side * gazeLength * DimensionUtil::vector (d);

      cam.set (cam.gazePoint (), toEyePoint, glm::vec3 (0.0f, 1.0f, 0.0f));
    };

    auto isSnapped = [&cam] (Dimension d) -> bool {
      const float dot = glm::abs (glm::dot ( glm::normalize (cam.toEyePoint ())
                                           , DimensionUtil::vector (d) ));
      return dot > 1.0f - Util::epsilon ();
    };

    if (key == Qt::Key_C) {
      if (mod == Qt::NoModifier) {
        if (isSnapped (Dimension::X)) {
          snapCamera (Dimension::Y, true);
        }
        else if (isSnapped (Dimension::Y)) {
          snapCamera (Dimension::Z, true);
        }
        else if (isSnapped (Dimension::Z)) {
          snapCamera (Dimension::X, true);
        }
        else {
          snapCamera (cam.primaryDimension (), false);
        }
      }
      else if (mod == Qt::ShiftModifier) {
        if (isSnapped (Dimension::X)) {
          snapCamera (Dimension::Z, true);
        }
        else if (isSnapped (Dimension::Y)) {
          snapCamera (Dimension::X, true);
        }
        else if (isSnapped (Dimension::Z)) {
          snapCamera (Dimension::Y, true);
        }
      }
      else if (mod == Qt::ControlModifier) {
        cam.set (glm::vec3 (0.0f), cam.position (), glm::vec3 (0.0f, 1.0f, 0.0f));
      }
      state.mainWindow ().glWidget ().update ();
    }
  }

  void mouseMoveEvent (State& state, const QMouseEvent& event) {
    if (event.buttons () == Qt::MiddleButton) {
            Camera&     cam        = state.camera ();
      const glm::uvec2& resolution = cam.resolution ();
            glm::ivec2  newPos     = ViewUtil::toIVec2 (event);
            glm::ivec2  delta      = newPos - oldPos;

      if (event.modifiers () == Qt::NoModifier) {
        if (delta.x != 0) {
          cam.verticalRotation ( 2.0f * glm::pi <float> () 
                               * this->rotationFactor
                               * float (-delta.x) / float (resolution.x));
        }
        if (delta.y != 0) {
          cam.horizontalRotation ( 2.0f * glm::pi <float> ()
                                 * this->rotationFactor
                                 * float (-delta.y) / float (resolution.y));
        }
      }
      else if (event.modifiers () == Qt::ShiftModifier) {
        cam.setGaze ( cam.gazePoint () 
                    + (this->panningFactor * float ( delta.x) * cam.right ())
                    + (this->panningFactor * float (-delta.y) * cam.up    ())
                    );
      }
      this->oldPos = newPos;
      state.mainWindow ().glWidget ().update ();
    }
  }

  void mousePressEvent (State& state, const QMouseEvent& event) {
    if (event.button () == Qt::MiddleButton) {
      this->oldPos = ViewUtil::toIVec2 (event);

      if (event.modifiers () == Qt::ControlModifier) {
        Camera& cam = state.camera ();
        WingedFaceIntersection intersection;
        if (state.scene ().intersects (cam.ray (ViewUtil::toIVec2 (event)), intersection)) {
          cam.set ( intersection.position ()
                  , cam.position () - intersection.position ()
                  , cam.up () );
          state.mainWindow ().glWidget ().update ();
        }
      }
    }
  }

  void wheelEvent (State& state, const QWheelEvent& event) {
    if (event.orientation () == Qt::Vertical) {
      if (event.delta () > 0) {
        state.camera ().stepAlongGaze (this->zoomInFactor);
      }
      else if (event.delta () < 0) {
        state.camera ().stepAlongGaze (1.0f / this->zoomInFactor);
      }
      state.mainWindow ().glWidget ().update ();
    }
  }

  void runFromConfig (const Config& config) {
    rotationFactor = config.get <float> ("editor/camera/rotation-factor");
    panningFactor  = config.get <float> ("editor/camera/panning-factor");
    zoomInFactor   = config.get <float> ("editor/camera/zoom-in-factor");
  }
};

DELEGATE1_BIG3 (ToolMoveCamera, const Config&)
DELEGATE2 (void, ToolMoveCamera, keyPressEvent, State&, const QKeyEvent&)
DELEGATE2 (void, ToolMoveCamera, mouseMoveEvent, State&, const QMouseEvent&)
DELEGATE2 (void, ToolMoveCamera, mousePressEvent, State&, const QMouseEvent&)
DELEGATE2 (void, ToolMoveCamera, wheelEvent, State&, const QWheelEvent&)
DELEGATE1 (void, ToolMoveCamera, runFromConfig, const Config&)

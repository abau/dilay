#include <QWheelEvent>
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include "camera.hpp"
#include "config.hpp"
#include "intersection.hpp"
#include "primitive/ray.hpp"
#include "scene.hpp"
#include "state.hpp"
#include "tool/move-camera.hpp"
#include "view/gl-widget.hpp"
#include "view/main-window.hpp"
#include "view/util.hpp"

struct ToolMoveCamera::Impl {
  glm::ivec2 oldPos;
  float      rotationFactor;
  float      panningFactor;
  float      zoomInFactor;

  Impl (const Config& config) {
    this->runFromConfig (config);
  }

  void mouseMoveEvent (State& state, QMouseEvent& event) {
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
      else if (event.modifiers () == Qt::ControlModifier) {
        cam.setGaze ( cam.gazePoint () 
                    + (this->panningFactor * float ( delta.x) * cam.right ())
                    + (this->panningFactor * float (-delta.y) * cam.up    ())
                    );
      }
      this->oldPos = newPos;
      state.mainWindow ().glWidget ().update ();
    }
  }

  void mousePressEvent (State& state, QMouseEvent& event) {
    if (event.button () == Qt::MiddleButton) {
      this->oldPos = ViewUtil::toIVec2 (event);

      if (event.modifiers () == Qt::ControlModifier) {
        Camera&      cam = state.camera ();
        Intersection intersection;
        if (state.scene ().intersects (cam.ray (ViewUtil::toIVec2 (event)), intersection)) {
          cam.set ( intersection.position ()
                  , cam.position () - intersection.position ()
                  , cam.up () );
          state.mainWindow ().glWidget ().update ();
        }
      }
    }
  }

  void wheelEvent (State& state, QWheelEvent& event) {
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
DELEGATE2 (void, ToolMoveCamera, mouseMoveEvent, State&, QMouseEvent&)
DELEGATE2 (void, ToolMoveCamera, mousePressEvent, State&, QMouseEvent&)
DELEGATE2 (void, ToolMoveCamera, wheelEvent, State&, QWheelEvent&)
DELEGATE1 (void, ToolMoveCamera, runFromConfig, const Config&)

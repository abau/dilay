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
  const float      rotationFactor;
  const float      panningFactor;
  const float      zoomInFactor;

  Impl ()
    : rotationFactor (Config::get <float> ("/config/editor/camera/rotation-factor"))
    , panningFactor  (Config::get <float> ("/config/editor/camera/panning-factor"))
    , zoomInFactor   (Config::get <float> ("/config/editor/camera/zoom-in-factor"))
  {}

  void mouseMoveEvent (QMouseEvent& event) {
    if (event.buttons ().testFlag (Qt::MiddleButton)) {
            Camera&     cam        = State::camera ();
      const glm::uvec2& resolution = cam.resolution ();
            glm::ivec2  newPos     = ViewUtil::toIVec2 (event);
            glm::ivec2  delta      = newPos - oldPos;

      if (event.modifiers ().testFlag (Qt::NoModifier)) {
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
      else if (event.modifiers ().testFlag (Qt::ControlModifier)) {
        cam.setGaze ( cam.gazePoint () 
                    + (this->panningFactor * float ( delta.x) * cam.right ())
                    + (this->panningFactor * float (-delta.y) * cam.up    ())
                    );
      }
      this->oldPos = newPos;
      State::mainWindow ().glWidget ().update ();
    }
  }

  void mousePressEvent (QMouseEvent& event) {
    if (event.button () == Qt::MiddleButton) {
      this->oldPos = ViewUtil::toIVec2 (event);
    }
  }

  void mouseReleaseEvent (QMouseEvent& event) {
    if (event.button () == Qt::MiddleButton && event.modifiers ().testFlag (Qt::ShiftModifier)) {
      Camera&      cam = State::camera ();
      Intersection intersection;
      if (State::scene ().intersects (cam.ray (ViewUtil::toIVec2 (event)), intersection)) {
        cam.setGaze (intersection.position ());
        State::mainWindow ().glWidget ().update ();
      }
    }
  }

  void wheelEvent (QWheelEvent& event) {
    if (event.orientation () == Qt::Vertical) {
      if (event.delta () > 0) {
        State::camera ().stepAlongGaze (this->zoomInFactor);
      }
      else if (event.delta () < 0) {
        State::camera ().stepAlongGaze (1.0f / this->zoomInFactor);
      }
      State::mainWindow ().glWidget ().update ();
    }
  }
};

DELEGATE_BIG3 (ToolMoveCamera)
DELEGATE1 (void, ToolMoveCamera, mouseMoveEvent, QMouseEvent&)
DELEGATE1 (void, ToolMoveCamera, mousePressEvent, QMouseEvent&)
DELEGATE1 (void, ToolMoveCamera, mouseReleaseEvent, QMouseEvent&)
DELEGATE1 (void, ToolMoveCamera, wheelEvent, QWheelEvent&)

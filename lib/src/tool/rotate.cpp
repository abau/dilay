#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include <QWheelEvent>
#include "tool/rotate.hpp"
#include "renderer.hpp"
#include "state.hpp"
#include "camera.hpp"
#include "view/util.hpp"
#include "view/main-window.hpp"
#include "view/gl-widget.hpp"
#include "view/tool-message.hpp"
#include "config.hpp"

struct ToolRotate::Impl {
        ToolRotate* self;
        glm::ivec2  oldPos;
  const glm::vec3   originalGazepoint;
  const glm::vec3   originalToEyepoint;
  const glm::vec3   originalUp;
  const float       rotationFactor;
  const float       panningFactor;

  Impl (ToolRotate* s)
    : self               (s)
    , oldPos             (s->clickPosition ())
    , originalGazepoint  (State::camera ().gazePoint ())
    , originalToEyepoint (State::camera ().toEyePoint ())
    , originalUp         (State::camera ().up ())
    , rotationFactor     (Config::get <float> ("/editor/camera/rotation-factor"))
    , panningFactor      (Config::get <float> ("/editor/camera/panning-factor"))
  {
    this->self->mainWindow ().showMessage (this->self->message ());
  }

  ~Impl () {
    if (State::hasTool ()) {
      this->self->mainWindow ().showMessage (State::tool ().message ());
    }
    else {
      this->self->mainWindow ().showDefaultMessage ();
    }
  }

  bool runMouseMoveEvent (QMouseEvent& event) {
          Camera&     cam        = State::camera ();
    const glm::uvec2& resolution = cam.resolution ();
          glm::ivec2  newPos     = ViewUtil::toIVec2 (event);
          glm::ivec2  delta      = newPos - oldPos;

    if (event.buttons ().testFlag (Qt::MiddleButton)) {
      cam.setGaze ( cam.gazePoint () 
                  + (this->panningFactor * float ( delta.x) * cam.right ())
                  + (this->panningFactor * float (-delta.y) * cam.up    ())
                  );
    }
    else if (event.buttons ().testFlag (Qt::NoButton)) {
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
    this->oldPos = newPos;
    return true;
  }

  bool runMouseReleaseEvent (QMouseEvent& event) {
    if (event.button () == Qt::LeftButton) {
      this->self->mainWindow ().glWidget ().invalidateToolRotate ();
      return false;
    }
    else if (event.button () == Qt::RightButton) {
      State::camera ().set (this->originalGazepoint, this->originalToEyepoint, this->originalUp);
      this->self->updateGlWidget ();
      this->self->mainWindow ().glWidget ().invalidateToolRotate ();
      return false;
    }
    return false;
  }

  bool runWheelEvent (QWheelEvent& event) {
    return staticWheelEvent (event);
  }

  bool static staticWheelEvent (QWheelEvent& event) {
    if (event.orientation () == Qt::Vertical) {
      const float zoomInFactor = Config::get <float> ("/editor/camera/zoom-in-factor");

      if (event.delta () > 0)
        State::camera ().stepAlongGaze (zoomInFactor);
      else if (event.delta () < 0)
        State::camera ().stepAlongGaze (1.0f / zoomInFactor);
    }
    return true;
  }

  QString runMessage () const {
    return ViewToolMessage::message 
      ({ ViewToolMessage ("Accept")     .left   () 
       , ViewToolMessage ("Drag To Pan").middle ()
       , ViewToolMessage ("Cancel")     .right  ()
       }); 
  }
};

DELEGATE_BIG3_BASE ( ToolRotate, (ViewMainWindow& w, const glm::ivec2& p)
                   , (this), Tool, (w,p) )
DELEGATE1        (bool   , ToolRotate, runMouseMoveEvent, QMouseEvent&)
DELEGATE1        (bool   , ToolRotate, runMouseReleaseEvent, QMouseEvent&)
DELEGATE1        (bool   , ToolRotate, runWheelEvent, QWheelEvent&)
DELEGATE1_STATIC (bool   , ToolRotate, staticWheelEvent, QWheelEvent&)
DELEGATE_CONST   (QString, ToolRotate, runMessage)

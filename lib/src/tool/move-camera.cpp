#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include <QWheelEvent>
#include "tool/move-camera.hpp"
#include "renderer.hpp"
#include "state.hpp"
#include "camera.hpp"
#include "view/util.hpp"
#include "view/main-window.hpp"
#include "view/gl-widget.hpp"
#include "view/tool-menu-parameters.hpp"
#include "view/tool-message.hpp"
#include "config.hpp"

struct ToolMoveCamera::Impl {
        ToolMoveCamera* self;
        glm::ivec2      oldPos;
  const glm::vec3       originalGazepoint;
  const glm::vec3       originalToEyepoint;
  const glm::vec3       originalUp;
  const float           rotationFactor;
  const float           panningFactor;

  Impl (ToolMoveCamera* s)
    : self               (s)
    , oldPos             (s->menuParameters ().clickPosition ())
    , originalGazepoint  (State::camera ().gazePoint ())
    , originalToEyepoint (State::camera ().toEyePoint ())
    , originalUp         (State::camera ().up ())
    , rotationFactor     (Config::get <float> ("/config/editor/camera/rotation-factor"))
    , panningFactor      (Config::get <float> ("/config/editor/camera/panning-factor"))
  {
    this->self->menuParameters ().mainWindow ().showMessage (this->self->message ());
  }

  ~Impl () {
    if (State::hasTool ()) {
      this->self->menuParameters ().mainWindow ().showMessage (State::tool ().message ());
    }
    else {
      this->self->menuParameters ().mainWindow ().showDefaultMessage ();
    }
  }

  ToolResponse runMouseMoveEvent (QMouseEvent& event) {
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
    return ToolResponse::Redraw;
  }

  ToolResponse runMouseReleaseEvent (QMouseEvent& event) {
    if (event.button () == Qt::LeftButton) {
      return ToolResponse::Terminate;
    }
    return ToolResponse::None;
  }

  static ToolResponse staticWheelEvent (QWheelEvent& event) {
    if (event.orientation () == Qt::Vertical) {
      const float zoomInFactor = Config::get <float> ("/config/editor/camera/zoom-in-factor");

      if (event.delta () > 0)
        State::camera ().stepAlongGaze (zoomInFactor);
      else if (event.delta () < 0)
        State::camera ().stepAlongGaze (1.0f / zoomInFactor);
      return ToolResponse::Redraw;
    }
    return ToolResponse::None;
  }

  QString runMessage () const {
    return ViewToolMessage::message 
      ({ ViewToolMessage ("Accept")     .left   () 
       , ViewToolMessage ("Drag To Pan").middle ()
       , ViewToolMessage ("Cancel")     .right  ()
       }); 
  }

  void runCancel () {
    State::camera ().set (this->originalGazepoint, this->originalToEyepoint, this->originalUp);
  }
};

DELEGATE_BIG3_BASE ( ToolMoveCamera, (const ViewToolMenuParameters& p)
                   , (this), Tool, (p) )
DELEGATE1        (ToolResponse, ToolMoveCamera, runMouseMoveEvent, QMouseEvent&)
DELEGATE1        (ToolResponse, ToolMoveCamera, runMouseReleaseEvent, QMouseEvent&)
DELEGATE1_STATIC (ToolResponse, ToolMoveCamera, staticWheelEvent, QWheelEvent&)
DELEGATE_CONST   (QString     , ToolMoveCamera, runMessage)
DELEGATE         (void        , ToolMoveCamera, runCancel)

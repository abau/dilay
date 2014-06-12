#include <glm/glm.hpp>
#include <QWheelEvent>
#include "tool/rotate.hpp"
#include "renderer.hpp"
#include "state.hpp"
#include "camera.hpp"
#include "view/util.hpp"
#include "view/main-window.hpp"
#include "view/gl-widget.hpp"

struct ToolRotate::Impl {
        ToolRotate* self;
        glm::ivec2  oldPos;
  const glm::vec3   originalGazepoint;
  const glm::vec3   originalToEyepoint;
  const glm::vec3   originalUp;

  Impl (ToolRotate* s)
    : self               (s)
    , oldPos             (s->clickPosition ())
    , originalGazepoint  (State::camera ().gazePoint ())
    , originalToEyepoint (State::camera ().toEyePoint ())
    , originalUp         (State::camera ().up ())
  {}

  bool runMouseMoveEvent (QMouseEvent& event) {
          Camera&     cam        = State::camera ();
    const glm::uvec2& resolution = cam.resolution ();
          glm::ivec2  newPos     = ViewUtil::toIVec2 (event);
          glm::ivec2  delta      = newPos - oldPos;

    if (delta.x != 0) {
      cam.verticalRotation (360.0f * float (-delta.x) / float (resolution.x));
    }
    if (delta.y != 0) {
      cam.horizontalRotation (360.0f * float (-delta.y) / float (resolution.y));
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
    if (event.orientation () == Qt::Vertical) {
      if (event.delta () > 0)
        State::camera ().stepAlongGaze (true);
      else if (event.delta () < 0)
        State::camera ().stepAlongGaze (false);
    }
    return true;
  }
};

DELEGATE_BIG3_BASE ( ToolRotate, (ViewMainWindow& w, const glm::ivec2& p)
                   , (this), Tool, (w,p) )
DELEGATE1 (bool, ToolRotate, runMouseMoveEvent, QMouseEvent&)
DELEGATE1 (bool, ToolRotate, runMouseReleaseEvent, QMouseEvent&)
DELEGATE1 (bool, ToolRotate, runWheelEvent, QWheelEvent&)

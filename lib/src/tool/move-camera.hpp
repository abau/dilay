#ifndef DILAY_TOOL_MOVE_CAMERA
#define DILAY_TOOL_MOVE_CAMERA

#include <glm/fwd.hpp>
#include "configurable.hpp"

class Config;
class State;
class QMouseEvent;
class QWheelEvent;

class ToolMoveCamera : public Configurable {
  public:
    DECLARE_BIG3 (ToolMoveCamera, const Config&)

    void mouseMoveEvent  (State&, QMouseEvent&);
    void mousePressEvent (State&, QMouseEvent&);
    void wheelEvent      (State&, QWheelEvent&);

  private:
    IMPLEMENTATION

    void runFromConfig (const Config&);
};

#endif

#ifndef DILAY_TOOL_MOVE_CAMERA
#define DILAY_TOOL_MOVE_CAMERA

#include <glm/fwd.hpp>

class QMouseEvent;
class QWheelEvent;

class ToolMoveCamera {
  public:
    DECLARE_BIG3 (ToolMoveCamera)

    void mouseMoveEvent    (QMouseEvent&);
    void mousePressEvent   (QMouseEvent&);
    void wheelEvent        (QWheelEvent&);

  private:
    IMPLEMENTATION
};

#endif

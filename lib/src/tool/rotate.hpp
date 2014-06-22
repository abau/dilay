#ifndef DILAY_TOOL_ROTATE
#define DILAY_TOOL_ROTATE

#include "tool.hpp"

class ToolRotate : public Tool {
  public:
    DECLARE_BIG3 (ToolRotate, ViewMainWindow&, const glm::ivec2&)

    static ToolResponse staticWheelEvent (QWheelEvent&);

  private:
    class Impl;
    Impl* impl;

    ToolResponse runMouseMoveEvent    (QMouseEvent&);
    ToolResponse runMouseReleaseEvent (QMouseEvent&);
    QString      runMessage           () const;
};

#endif

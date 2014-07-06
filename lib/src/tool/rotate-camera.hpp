#ifndef DILAY_TOOL_ROTATE_CAMERA
#define DILAY_TOOL_ROTATE_CAMERA

#include "tool.hpp"

class ToolRotateCamera : public Tool {
  public:
    DECLARE_BIG3 (ToolRotateCamera, const ViewToolMenuParameters&)

    static ToolResponse staticWheelEvent (QWheelEvent&);

  private:
    class Impl;
    Impl* impl;

    ToolResponse runMouseMoveEvent    (QMouseEvent&);
    ToolResponse runMouseReleaseEvent (QMouseEvent&);
    QString      runMessage           () const;
    void         runCancel            ();
};

#endif

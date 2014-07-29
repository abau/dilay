#ifndef DILAY_TOOL_MOVE_CAMERA
#define DILAY_TOOL_MOVE_CAMERA

#include "tool.hpp"

class ToolMoveCamera : public Tool {
  public:
    DECLARE_BIG3 (ToolMoveCamera, const ViewToolMenuParameters&)

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

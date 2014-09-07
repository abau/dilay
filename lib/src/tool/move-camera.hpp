#ifndef DILAY_TOOL_MOVE_CAMERA
#define DILAY_TOOL_MOVE_CAMERA

#include "tool.hpp"

class ToolMoveCamera : public Tool {
  public:
    DECLARE_BIG3 (ToolMoveCamera, const ViewToolMenuParameters&, bool)

    static ToolResponse staticWheelEvent (QWheelEvent&);

  private:
    IMPLEMENTATION

    ToolResponse runInitialize        ();
    ToolResponse runMouseMoveEvent    (QMouseEvent&);
    ToolResponse runMouseReleaseEvent (QMouseEvent&);
    QString      runMessage           () const;
    void         runCancel            ();
};

#endif

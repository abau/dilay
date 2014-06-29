#ifndef DILAY_TOOL_ROTATE
#define DILAY_TOOL_ROTATE

#include "tool.hpp"

class ToolRotate : public Tool {
  public:
    DECLARE_BIG3 (ToolRotate, const ViewToolMenuParameters&)

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

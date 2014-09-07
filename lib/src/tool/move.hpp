#ifndef DILAY_TOOL_MOVE
#define DILAY_TOOL_MOVE

#include "tool.hpp"

enum class Movement;

class ToolMove : public Tool {
  public:
    DECLARE_BIG3 (ToolMove, const ViewToolMenuParameters&, Movement)

    static QString toolName (Movement);

  private:
    IMPLEMENTATION

    void         runClose             ();
    void         runCancel            ();
    ToolResponse runMouseMoveEvent    (QMouseEvent&);
    ToolResponse runMouseReleaseEvent (QMouseEvent&);
};

#endif

#ifndef DILAY_TOOL_CARVE
#define DILAY_TOOL_CARVE

#include "tool.hpp"

class ToolCarve : public Tool {
  public:
    DECLARE_BIG3 (ToolCarve, const ViewToolMenuParameters&)

    static QString toolName ();

  private:
    class Impl;
    Impl* impl;

    ToolResponse runMouseReleaseEvent (QMouseEvent&);
};

#endif

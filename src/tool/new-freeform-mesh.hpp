#ifndef DILAY_TOOL_NEW_FREEFORM_MESH
#define DILAY_TOOL_NEW_FREEFORM_MESH

#include "tool.hpp"

class ToolNewFreeformMesh : public Tool {
  public:
    DECLARE_TOOL (ToolNewFreeformMesh)

  private:
    class Impl;
    Impl* impl;

    void runRender            ();
    bool runMouseMoveEvent    (QMouseEvent*);
    bool runMousePressEvent   (QMouseEvent*);
    bool runMouseReleaseEvent (QMouseEvent*);
};

#endif

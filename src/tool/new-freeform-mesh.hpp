#ifndef DILAY_TOOL_NEW_FREEFORM_MESH
#define DILAY_TOOL_NEW_FREEFORM_MESH

#include "macro.hpp"
#include "tool.hpp"

class ToolNewFreeformMesh : public Tool {
  public:
    DECLARE_BIG3 (ToolNewFreeformMesh)

  private:
    class Impl;
    Impl* impl;

    void         runInitialize      (ViewMainWindow*,QContextMenuEvent*);
    void         runRender          ();
    ToolResponse runMouseMoveEvent  (QMouseEvent*);
    ToolResponse runMousePressEvent (QMouseEvent*);
};

#endif

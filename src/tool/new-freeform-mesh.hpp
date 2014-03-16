#ifndef DILAY_TOOL_NEW_FREEFORM_MESH
#define DILAY_TOOL_NEW_FREEFORM_MESH

#include "macro.hpp"
#include "tool.hpp"

class ToolNewFreeformMesh : public Tool {
  public:
    DECLARE_BIG3 (ToolNewFreeformMesh)

    static QString toolName ();

  private:
    class Impl;
    Impl* impl;

    void runInitialize      (QContextMenuEvent*);
    void runRender          ();
    void runMouseMoveEvent  (QMouseEvent*);
    void runMousePressEvent (QMouseEvent*);
    void runClose           ();
};

#endif

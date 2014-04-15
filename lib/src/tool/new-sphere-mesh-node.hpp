#ifndef DILAY_TOOL_MOVE_SPHERE_MESH_NODE
#define DILAY_TOOL_MOVE_SPHERE_MESH_NODE

#include "tool.hpp"

class ToolMoveSphereMeshNode : public Tool {
  public:
    DECLARE_TOOL (ToolMoveSphereMeshNode, bool = false)

  private:
    class Impl;
    Impl* impl;

    void runRender          ();
    void runMouseMoveEvent  (QMouseEvent*);
    void runMousePressEvent (QMouseEvent*);
};

#endif

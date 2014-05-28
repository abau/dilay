#ifndef DILAY_TOOL_MODIFY_SPHERE_MESH_NODE
#define DILAY_TOOL_MODIFY_SPHERE_MESH_NODE

#include "tool.hpp"

class ToolModifySphereMeshNode : public Tool {
  public:
    enum class Mode { NewMesh, NewNode, MoveNode };

    DECLARE_BIG3 (ToolModifySphereMeshNode, ViewMainWindow*, QContextMenuEvent*, Mode)

    static QString toolName (Mode);

  private:
    class Impl;
    Impl* impl;
};

#endif

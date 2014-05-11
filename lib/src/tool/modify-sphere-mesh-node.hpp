#ifndef DILAY_TOOL_MODIFY_SPHERE_MESH_NODE
#define DILAY_TOOL_MODIFY_SPHERE_MESH_NODE

#include "tool.hpp"

class ToolModifySphereMeshNode : public Tool {
  public:
    enum class Mode { NewMesh, NewNode, MoveNode };

    DECLARE_TOOL (ToolModifySphereMeshNode, Mode)

  private:
    class Impl;
    Impl* impl;
};

#endif

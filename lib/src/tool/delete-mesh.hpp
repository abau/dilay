#ifndef DILAY_TOOL_DELETE_MESH
#define DILAY_TOOL_DELETE_MESH

#include "tool.hpp"

enum class MeshType;

class ToolDeleteMesh : public Tool {
  public:
    DECLARE_BIG3 (ToolDeleteMesh, ViewMainWindow&, const glm::ivec2&, MeshType)

    static QString toolName (MeshType);

  private:
    class Impl;
    Impl* impl;

    ToolResponse runExecute ();
};

#endif

#ifndef DILAY_TOOL_SUBDIVIDE_MESH
#define DILAY_TOOL_SUBDIVIDE_MESH

#include "tool.hpp"

class ToolSubdivideMesh : public Tool {
  public:
    DECLARE_BIG3 (ToolSubdivideMesh, const ViewToolMenuParameters&)

    static QString toolName ();

  private:
    IMPLEMENTATION

    ToolResponse runInitialize ();
};

#endif

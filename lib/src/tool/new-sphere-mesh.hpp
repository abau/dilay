#ifndef DILAY_TOOL_NEW_SPHERE_MESH
#define DILAY_TOOL_NEW_SPHERE_MESH

#include "tool.hpp"

class ToolNewSphereMesh : public Tool {
  public:
    DECLARE_BIG3 (ToolNewSphereMesh, const ViewToolMenuParameters&)

    static QString toolName ();

  private:
    class Impl;
    Impl* impl;

    ToolResponse runInitialize ();
};

#endif

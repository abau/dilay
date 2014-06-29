#ifndef DILAY_TOOL_NEW_FREEFORM_MESH
#define DILAY_TOOL_NEW_FREEFORM_MESH

#include "tool.hpp"

class ToolNewFreeformMesh : public Tool {
  public:
    DECLARE_BIG3 (ToolNewFreeformMesh, const ViewToolMenuParameters&)

    static QString toolName ();

  private:
    class Impl;
    Impl* impl;

    void         runRender     ();
    ToolResponse runInitialize ();
    void         runApply      ();
};

#endif

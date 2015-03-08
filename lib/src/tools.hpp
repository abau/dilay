#ifndef DILAY_TOOLS
#define DILAY_TOOLS

#include "tool.hpp"
#include "tool/sculpt.hpp"

DECLARE_TOOL (ToolMoveMesh, "move", DECLARE_TOOL_RUN_MOUSE_MOVE_EVENT
                                    DECLARE_TOOL_RUN_MOUSE_PRESS_EVENT
                                    DECLARE_TOOL_RUN_MOUSE_RELEASE_EVENT )

DECLARE_TOOL (ToolDeleteMesh, "delete-mesh", DECLARE_TOOL_RUN_MOUSE_RELEASE_EVENT)

DECLARE_TOOL (ToolNewMesh, "new-mesh", DECLARE_TOOL_RUN_RENDER
                                       DECLARE_TOOL_RUN_CLOSE )

DECLARE_TOOL_SCULPT (ToolSculptCarve, "sculpt/carve")
DECLARE_TOOL_SCULPT (ToolSculptGrab, "sculpt/grab")
DECLARE_TOOL_SCULPT (ToolSculptSmooth, "sculpt/smooth")

#endif

#ifndef DILAY_TOOLS
#define DILAY_TOOLS

#include "tool.hpp"

DECLARE_TOOL (ToolMove, "move", DECLARE_TOOL_RUN_CLOSE
                                DECLARE_TOOL_RUN_MOUSE_MOVE_EVENT
                                DECLARE_TOOL_RUN_MOUSE_PRESS_EVENT )

DECLARE_TOOL (ToolSculpt, "sculpt", ALLOW_TOOL_UNDO_REDO
                                    DECLARE_TOOL_RUN_RENDER
                                    DECLARE_TOOL_RUN_MOUSE_MOVE_EVENT
                                    DECLARE_TOOL_RUN_MOUSE_PRESS_EVENT
                                    DECLARE_TOOL_RUN_MOUSE_RELEASE_EVENT
                                    DECLARE_TOOL_RUN_MOUSE_WHEEL_EVENT
                                    DECLARE_TOOL_RUN_CLOSE )

DECLARE_TOOL (ToolDeleteMesh, "delete-mesh", DECLARE_TOOL_RUN_INITIALIZE)

DECLARE_TOOL (ToolNewWingedMesh, "new-winged-mesh", DECLARE_TOOL_RUN_RENDER
                                                    DECLARE_TOOL_RUN_CLOSE )

DECLARE_TOOL (ToolSubdivideMesh, "subdivide-mesh", DECLARE_TOOL_RUN_INITIALIZE)

#endif

#ifndef DILAY_TOOLS
#define DILAY_TOOLS

#include "tool.hpp"

enum class MeshType;

DECLARE_TOOL (ToolMove, DECLARE_TOOL_RUN_CLOSE
                        DECLARE_TOOL_RUN_MOUSE_MOVE_EVENT
                        DECLARE_TOOL_RUN_MOUSE_PRESS_EVENT )

DECLARE_TOOL (ToolCarve, DECLARE_TOOL_RUN_RENDER
                         DECLARE_TOOL_RUN_MOUSE_MOVE_EVENT )

DECLARE_PARAM_TOOL (ToolDeleteMesh, (MeshType), DECLARE_TOOL_RUN_INITIALIZE)

DECLARE_TOOL (ToolNewFreeformMesh, DECLARE_TOOL_RUN_CLOSE
                                   DECLARE_TOOL_RUN_RENDER )

DECLARE_TOOL (ToolSubdivideMesh, DECLARE_TOOL_RUN_INITIALIZE)

#endif

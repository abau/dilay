/* This file is part of Dilay
 * Copyright © 2015 Alexander Bau
 * Use and redistribute under the terms of the GNU General Public License
 */
#ifndef DILAY_TOOLS
#define DILAY_TOOLS

#include "tool.hpp"
#include "tool/sculpt.hpp"

DECLARE_TOOL (ToolMoveMesh, "move", DECLARE_TOOL_RUN_MOUSE_MOVE_EVENT
                                    DECLARE_TOOL_RUN_MOUSE_PRESS_EVENT
                                    DECLARE_TOOL_RUN_MOUSE_RELEASE_EVENT )

DECLARE_TOOL (ToolDeleteMesh, "delete-mesh", DECLARE_TOOL_RUN_MOUSE_RELEASE_EVENT)

DECLARE_TOOL (ToolNewMesh, "new-mesh", DECLARE_TOOL_RUN_INITIALIZE)

DECLARE_TOOL_SCULPT (ToolSculptCarve  , "sculpt/carve")
DECLARE_TOOL_SCULPT (ToolSculptDrag   , "sculpt/drag")
DECLARE_TOOL_SCULPT (ToolSculptGrab   , "sculpt/grab")
DECLARE_TOOL_SCULPT (ToolSculptSmooth , "sculpt/smooth")
DECLARE_TOOL_SCULPT (ToolSculptFlatten, "sculpt/flatten")
DECLARE_TOOL_SCULPT (ToolSculptCrease , "sculpt/crease")
DECLARE_TOOL_SCULPT (ToolSculptPinch  , "sculpt/pinch")
DECLARE_TOOL_SCULPT (ToolSculptReduce , "sculpt/reduce")

DECLARE_TOOL (ToolNewSketch, "new-sketch", DECLARE_TOOL_RUN_INITIALIZE)

#endif

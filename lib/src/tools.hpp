/* This file is part of Dilay
 * Copyright © 2015-2017 Alexander Bau
 * Use and redistribute under the terms of the GNU General Public License
 */
#ifndef DILAY_TOOLS
#define DILAY_TOOLS

#include "tool.hpp"
#include "tool/sculpt.hpp"

DECLARE_TOOL (ToolMoveMesh, "move", DECLARE_TOOL_RUN_MOVE_EVENT DECLARE_TOOL_RUN_PRESS_EVENT
                                      DECLARE_TOOL_RUN_RELEASE_EVENT)

DECLARE_TOOL (ToolDeleteMesh, "delete-mesh", DECLARE_TOOL_RUN_RELEASE_EVENT)

DECLARE_TOOL (ToolNewMesh, "new-mesh", DECLARE_TOOL_RUN_INITIALIZE)

DECLARE_TOOL_SCULPT (ToolSculptDraw, "sculpt/draw")
DECLARE_TOOL_SCULPT (ToolSculptDrag, "sculpt/drag")
DECLARE_TOOL_SCULPT (ToolSculptGrab, "sculpt/grab")
DECLARE_TOOL_SCULPT (ToolSculptSmooth, "sculpt/smooth")
DECLARE_TOOL_SCULPT (ToolSculptFlatten, "sculpt/flatten")
DECLARE_TOOL_SCULPT (ToolSculptCrease, "sculpt/crease")
DECLARE_TOOL_SCULPT (ToolSculptPinch, "sculpt/pinch")
DECLARE_TOOL_SCULPT (ToolSculptReduce, "sculpt/reduce")

DECLARE_TOOL (ToolNewSketch, "new-sketch", DECLARE_TOOL_RUN_INITIALIZE)

DECLARE_TOOL (ToolModifySketch, "modify-sketch",
              DECLARE_TOOL_RUN_MOVE_EVENT DECLARE_TOOL_RUN_PRESS_EVENT
                                          DECLARE_TOOL_RUN_RELEASE_EVENT)

DECLARE_TOOL (ToolDeleteSketch, "delete-sketch", DECLARE_TOOL_RUN_RELEASE_EVENT)

DECLARE_TOOL (ToolRebalanceSketch, "rebalance-sketch",
              DECLARE_TOOL_RUN_RELEASE_EVENT DECLARE_TOOL_RUN_CLOSE)

DECLARE_TOOL (ToolConvertSketch, "convert-sketch", DECLARE_TOOL_RUN_RELEASE_EVENT)

DECLARE_TOOL (ToolSketchSpheres, "sketch-spheres",
              DECLARE_TOOL_RUN_INITIALIZE DECLARE_TOOL_RUN_RENDER DECLARE_TOOL_RUN_MOVE_EVENT
                DECLARE_TOOL_RUN_PRESS_EVENT DECLARE_TOOL_RUN_RELEASE_EVENT
                  DECLARE_TOOL_RUN_MOUSE_WHEEL_EVENT DECLARE_TOOL_RUN_CURSOR_UPDATE
                                                     DECLARE_TOOL_RUN_FROM_CONFIG)

DECLARE_TOOL (ToolTrimMesh, "trim-mesh", DECLARE_TOOL_RUN_MOVE_EVENT DECLARE_TOOL_RUN_PRESS_EVENT
                                           DECLARE_TOOL_RUN_RELEASE_EVENT DECLARE_TOOL_RUN_PAINT)
#endif

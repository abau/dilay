#ifndef DILAY_TOOL_SCULPT_BEHAVIORS
#define DILAY_TOOL_SCULPT_BEHAVIORS

#include "tool/sculpt/behavior.hpp"

DECLARE_TOOL_SCULPT_BEHAVIOR (ToolSculptCarve, "carve", )
DECLARE_TOOL_SCULPT_BEHAVIOR (ToolSculptDrag, "drag",
                                DECLARE_TOOL_SCULPT_BEHAVIOR_RUN_MOUSE_LEFT_RELEASE_EVENT
                                DECLARE_TOOL_SCULPT_BEHAVIOR_RUN_RENDER )

#endif

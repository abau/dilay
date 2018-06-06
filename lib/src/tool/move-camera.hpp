/* This file is part of Dilay
 * Copyright © 2015-2018 Alexander Bau
 * Use and redistribute under the terms of the GNU General Public License
 */
#ifndef DILAY_TOOL_MOVE_CAMERA
#define DILAY_TOOL_MOVE_CAMERA

#include "tool.hpp"

class ToolMoveCamera : public Tool
{
public:
  DECLARE_BIG2 (ToolMoveCamera, State&)

  ToolMoveCamera (State&, bool);

  ToolKey getKey () const { return ToolKey::MoveCamera; }

  void snap ();
  void resetGazePoint ();

private:
  IMPLEMENTATION

  ToolResponse runInitialize ();
  ToolResponse runMoveEvent (const ViewPointingEvent&);
  ToolResponse runPressEvent (const ViewPointingEvent&);
  ToolResponse runWheelEvent (const QWheelEvent&);
  void         runFromConfig ();
};

#endif

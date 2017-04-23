/* This file is part of Dilay
 * Copyright © 2015-2017 Alexander Bau
 * Use and redistribute under the terms of the GNU General Public License
 */
#ifndef DILAY_TOOL_SCULPT_ACTION
#define DILAY_TOOL_SCULPT_ACTION

class DynamicMesh;
class SculptBrush;

namespace ToolSculptAction
{
  void sculpt (const SculptBrush&);
  void smoothMesh (DynamicMesh&);
  void deleteDegeneratedFaces (DynamicMesh&);
};

#endif

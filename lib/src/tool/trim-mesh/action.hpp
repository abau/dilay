/* This file is part of Dilay
 * Copyright © 2015-2017 Alexander Bau
 * Use and redistribute under the terms of the GNU General Public License
 */
#ifndef DILAY_TOOL_TRIM_MESH_ACTION
#define DILAY_TOOL_TRIM_MESH_ACTION

class ToolTrimMeshBorder;
class WingedMesh;

namespace ToolTrimMeshAction {

  bool trimMesh (WingedMesh&, ToolTrimMeshBorder&);
}

#endif

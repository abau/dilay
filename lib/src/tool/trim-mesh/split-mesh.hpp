/* This file is part of Dilay
 * Copyright © 2015-2017 Alexander Bau
 * Use and redistribute under the terms of the GNU General Public License
 */
#ifndef DILAY_TOOL_TRIM_MESH_SPLIT_MESH
#define DILAY_TOOL_TRIM_MESH_SPLIT_MESH

class ToolTrimMeshBorder;
class WingedMesh;

namespace ToolTrimMeshSplitMesh {

  bool splitMesh (WingedMesh&, ToolTrimMeshBorder&);
}

#endif

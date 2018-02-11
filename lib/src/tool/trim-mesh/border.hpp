/* This file is part of Dilay
 * Copyright © 2015-2018 Alexander Bau
 * Use and redistribute under the terms of the GNU General Public License
 */
#ifndef DILAY_TOOL_TRIM_MESH_BORDER
#define DILAY_TOOL_TRIM_MESH_BORDER

#include <glm/fwd.hpp>
#include <vector>
#include "macro.hpp"

class DynamicMesh;
class PrimPlane;
class PrimRay;

class ToolTrimMeshBorder
{
public:
  typedef std::vector<unsigned int> Polyline;
  typedef std::vector<Polyline>     Polylines;

  DECLARE_BIG2 (ToolTrimMeshBorder, DynamicMesh&, const PrimRay&, const PrimRay&)

  DynamicMesh&     mesh () const;
  const Polylines& polylines () const;
  const PrimPlane& plane () const;
  void             addVertex (unsigned int, const glm::vec3&);
  void             addPolyline ();
  void             setNewIndices (const std::vector<unsigned int>&);
  bool             onBorder (const glm::vec3&) const;
  bool             intersects (const PrimRay&, float&) const;
  void             deleteEmptyPolylines ();
  bool             hasVertices () const;

private:
  IMPLEMENTATION
};
#endif

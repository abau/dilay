/* This file is part of Dilay
 * Copyright © 2015-2017 Alexander Bau
 * Use and redistribute under the terms of the GNU General Public License
 */
#ifndef DILAY_TOOL_TRIM_MESH_BORDER
#define DILAY_TOOL_TRIM_MESH_BORDER

#include <glm/fwd.hpp>
#include <vector>
#include "macro.hpp"

class Camera;
class PrimPlane;
class PrimRay;

class ToolTrimMeshBorderSegment
{
public:
  typedef std::vector<unsigned int> Polyline;
  typedef std::vector<Polyline>     Polylines;

  DECLARE_BIG3 (ToolTrimMeshBorderSegment, const PrimRay&, const PrimRay&)

  ToolTrimMeshBorderSegment (const PrimPlane&, const PrimRay&);
  ToolTrimMeshBorderSegment (const PrimRay&, const PrimPlane&);
  ToolTrimMeshBorderSegment (const PrimPlane&);

  const Polylines& polylines () const;
  const PrimPlane& plane () const;
  const PrimRay&   edge () const;
  void             addVertex (unsigned int, const glm::vec3&);
  void             addPolyline ();
  void             setNewIndices (const std::vector<unsigned int>&);
  bool             onBorder (const glm::vec3&, bool* = nullptr) const;
  bool             intersects (const PrimRay&, float&) const;
  void             deleteEmptyPolylines ();

private:
  IMPLEMENTATION
};

class ToolTrimMeshBorder
{
public:
  DECLARE_BIG2 (ToolTrimMeshBorder, const Camera&, const std::vector<glm::ivec2>&)

  const ToolTrimMeshBorderSegment& segment (unsigned int) const;
  const ToolTrimMeshBorderSegment& getSegment (const glm::vec3&, const glm::vec3&) const;

  unsigned int numSegments () const;
  void         addVertex (unsigned int, const glm::vec3&);
  void         addPolyline ();
  void         setNewIndices (const std::vector<unsigned int>&);
  bool         onBorder (const glm::vec3&) const;
  bool         trimVertex (const glm::vec3&) const;
  bool         trimFace (const glm::vec3&, const glm::vec3&, const glm::vec3&) const;
  void         deleteEmptyPolylines ();

private:
  IMPLEMENTATION
};
#endif

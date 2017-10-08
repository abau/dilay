/* This file is part of Dilay
 * Copyright © 2015-2017 Alexander Bau
 * Use and redistribute under the terms of the GNU General Public License
 */
#include "intersection.hpp"
#include "primitive/plane.hpp"
#include "primitive/ray.hpp"
#include "tool/trim-mesh/border.hpp"
#include "util.hpp"

namespace
{
  typedef ToolTrimMeshBorder::Polyline  Polyline;
  typedef ToolTrimMeshBorder::Polylines Polylines;
}

struct ToolTrimMeshBorder::Impl
{
  DynamicMesh&    mesh;
  Polylines       polylines;
  const PrimRay   edge1;
  const PrimRay   edge2;
  const PrimPlane plane;

  Impl (DynamicMesh& m, const PrimRay& r1, const PrimRay& r2)
    : mesh (m)
    , edge1 (r1)
    , edge2 (r2)
    , plane (r1.origin (), glm::cross (r2.direction (), r1.direction ()))
  {
  }

  void addVertex (unsigned int index, const glm::vec3& p)
  {
    assert (this->onBorder (p));
    assert (this->polylines.empty () == false);
    unused (p);

    this->polylines.back ().emplace_back (index);
  }

  void addPolyline () { this->polylines.emplace_back (); }

  void setNewIndices (const std::vector<unsigned int>& newIndices)
  {
    for (Polyline& p : this->polylines)
    {
      for (unsigned int& i : p)
      {
        assert (newIndices.at (i) != Util::invalidIndex ());

        i = newIndices.at (i);
      }
    }
  }

  bool isValidProjection (const glm::vec3& p) const
  {
    // assert (this->plane.onPlane (p)); // Occasionally fails due to rounding errors (?)

    const bool c1 = 0.0f < glm::dot (this->plane.normal (), glm::cross (p - this->edge1.origin (),
                                                                        this->edge1.direction ()));

    const bool c2 = 0.0f < glm::dot (this->plane.normal (), glm::cross (this->edge2.direction (),
                                                                        p - this->edge2.origin ()));
    return c1 && c2;
  }

  bool onBorder (const glm::vec3& p) const
  {
    if (this->edge1.onRay (p))
    {
      return true;
    }
    else if (this->edge2.onRay (p))
    {
      return true;
    }
    else if (this->plane.onPlane (p))
    {
      return this->isValidProjection (p);
    }
    else
    {
      return false;
    }
  }

  bool intersects (const PrimRay& ray, float& t) const
  {
    if (IntersectionUtil::intersects (ray, this->plane, &t))
    {
      return this->isValidProjection (ray.pointAt (t));
    }
    else
    {
      return false;
    }
  }

  void deleteEmptyPolylines ()
  {
    this->polylines.erase (std::remove_if (this->polylines.begin (), this->polylines.end (),
                                           [](Polyline& p) { return p.empty (); }),
                           this->polylines.end ());
  }

  bool hasVertices () const
  {
    for (const Polyline& p : this->polylines)
    {
      if (p.empty () == false)
      {
        return true;
      }
    }
    return false;
  }
};

DELEGATE3_BIG2 (ToolTrimMeshBorder, DynamicMesh&, const PrimRay&, const PrimRay&)
GETTER_CONST (DynamicMesh&, ToolTrimMeshBorder, mesh)
GETTER_CONST (const Polylines&, ToolTrimMeshBorder, polylines)
GETTER_CONST (const PrimPlane&, ToolTrimMeshBorder, plane)
DELEGATE2 (void, ToolTrimMeshBorder, addVertex, unsigned int, const glm::vec3&)
DELEGATE (void, ToolTrimMeshBorder, addPolyline)
DELEGATE1 (void, ToolTrimMeshBorder, setNewIndices, const std::vector<unsigned int>&)
DELEGATE1_CONST (bool, ToolTrimMeshBorder, onBorder, const glm::vec3&)
DELEGATE2_CONST (bool, ToolTrimMeshBorder, intersects, const PrimRay&, float&)
DELEGATE (void, ToolTrimMeshBorder, deleteEmptyPolylines)
DELEGATE_CONST (bool, ToolTrimMeshBorder, hasVertices)

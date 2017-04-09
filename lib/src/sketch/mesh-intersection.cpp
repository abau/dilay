/* This file is part of Dilay
 * Copyright © 2015,2016 Alexander Bau
 * Use and redistribute under the terms of the GNU General Public License
 */
#include "sketch/mesh-intersection.hpp"
#include "sketch/mesh.hpp"

struct SketchMeshIntersection::Impl
{
  SketchMeshIntersection* self;
  SketchMesh*             _mesh;

  Impl (SketchMeshIntersection* s)
    : self (s)
  {
  }

  bool update (float d, const glm::vec3& p, const glm::vec3& n, SketchMesh& mesh)
  {
    if (this->self->Intersection::update (d, p, n))
    {
      this->_mesh = &mesh;
      return true;
    }
    else
    {
      return false;
    }
  }

  SketchMesh& mesh () const
  {
    assert (this->self->isIntersection ());
    assert (this->_mesh);
    return *this->_mesh;
  }
};

DELEGATE_BIG6_BASE (SketchMeshIntersection, (), (this), Intersection, ())
DELEGATE4 (bool, SketchMeshIntersection, update, float, const glm::vec3&, const glm::vec3&,
           SketchMesh&)
DELEGATE_CONST (SketchMesh&, SketchMeshIntersection, mesh)

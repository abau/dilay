/* This file is part of Dilay
 * Copyright © 2015-2017 Alexander Bau
 * Use and redistribute under the terms of the GNU General Public License
 */
#include "sketch/path-intersection.hpp"
#include "sketch/path.hpp"

struct SketchPathIntersection::Impl
{
  SketchPathIntersection* self;
  SketchPath*             _path;

  Impl (SketchPathIntersection* s)
    : self (s)
  {
  }

  bool update (float d, const glm::vec3& p, const glm::vec3& n, SketchMesh& mesh, SketchPath& path)
  {
    if (this->self->SketchMeshIntersection::update (d, p, n, mesh))
    {
      this->_path = &path;
      return true;
    }
    else
    {
      return false;
    }
  }

  SketchPath& path () const
  {
    assert (this->self->isIntersection ());
    assert (this->_path);
    return *this->_path;
  }
};

DELEGATE_BIG6_BASE (SketchPathIntersection, (), (this), SketchMeshIntersection, ())
DELEGATE5 (bool, SketchPathIntersection, update, float, const glm::vec3&, const glm::vec3&,
           SketchMesh&, SketchPath&)
DELEGATE_CONST (SketchPath&, SketchPathIntersection, path)

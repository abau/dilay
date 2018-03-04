/* This file is part of Dilay
 * Copyright © 2015-2018 Alexander Bau
 * Use and redistribute under the terms of the GNU General Public License
 */
#include "sketch/path-intersection.hpp"
#include "sketch/path.hpp"

SketchPathIntersection::SketchPathIntersection ()
  : _path (nullptr)
{
}

bool SketchPathIntersection::update (float d, const glm::vec3& p, const glm::vec3& n,
                                     SketchMesh& mesh, SketchPath& path)
{
  if (this->SketchMeshIntersection::update (d, p, n, mesh))
  {
    this->_path = &path;
    return true;
  }
  else
  {
    return false;
  }
}

SketchPath& SketchPathIntersection::path () const
{
  assert (this->isIntersection ());
  assert (this->_path);
  return *this->_path;
}

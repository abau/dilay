/* This file is part of Dilay
 * Copyright © 2015-2018 Alexander Bau
 * Use and redistribute under the terms of the GNU General Public License
 */
#include "sketch/mesh-intersection.hpp"
#include "sketch/mesh.hpp"

SketchMeshIntersection::SketchMeshIntersection ()
  : _mesh (nullptr)
{
}

bool SketchMeshIntersection::update (float d, const glm::vec3& p, const glm::vec3& n,
                                     SketchMesh& mesh)
{
  if (this->Intersection::update (d, p, n))
  {
    this->_mesh = &mesh;
    return true;
  }
  else
  {
    return false;
  }
}

SketchMesh& SketchMeshIntersection::mesh () const
{
  assert (this->isIntersection ());
  assert (this->_mesh);
  return *this->_mesh;
}

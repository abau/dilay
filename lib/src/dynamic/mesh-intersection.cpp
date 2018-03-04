/* This file is part of Dilay
 * Copyright © 2015-2018 Alexander Bau
 * Use and redistribute under the terms of the GNU General Public License
 */
#include "dynamic/mesh-intersection.hpp"
#include "dynamic/mesh.hpp"
#include "util.hpp"

DynamicMeshIntersection::DynamicMeshIntersection ()
  : _faceIndex (Util::invalidIndex ())
  , _mesh (nullptr)
{
}

bool DynamicMeshIntersection::update (float d, const glm::vec3& p, const glm::vec3& n,
                                      unsigned int i, DynamicMesh& mesh)
{
  if (this->Intersection::update (d, p, n))
  {
    this->_mesh = &mesh;
    this->_faceIndex = i;
    return true;
  }
  else
  {
    return false;
  }
}

unsigned int DynamicMeshIntersection::faceIndex () const
{
  assert (this->isIntersection ());
  return this->_faceIndex;
}

DynamicMesh& DynamicMeshIntersection::mesh () const
{
  assert (this->isIntersection ());
  assert (this->_mesh);
  return *this->_mesh;
}

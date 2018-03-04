/* This file is part of Dilay
 * Copyright © 2015-2018 Alexander Bau
 * Use and redistribute under the terms of the GNU General Public License
 */
#include "sketch/bone-intersection.hpp"
#include "sketch/mesh.hpp"

SketchBoneIntersection::SketchBoneIntersection ()
  : _child (nullptr)
{
}

bool SketchBoneIntersection::update (float d, const glm::vec3& p, const glm::vec3& projP,
                                     const glm::vec3& n, SketchMesh& mesh, SketchNode& child)
{
  assert (child.parent ());

  if (this->SketchMeshIntersection::update (d, p, n, mesh))
  {
    this->_child = &child;
    this->_projectedPosition = projP;
    return true;
  }
  else
  {
    return false;
  }
}

SketchNode& SketchBoneIntersection::parent () const
{
  assert (this->isIntersection ());
  assert (this->_child);
  return *this->_child->parent ();
}

SketchNode& SketchBoneIntersection::child () const
{
  assert (this->isIntersection ());
  assert (this->_child);
  return *this->_child;
}

const glm::vec3& SketchBoneIntersection::projectedPosition () const
{
  assert (this->isIntersection ());
  return this->_projectedPosition;
}

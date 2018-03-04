/* This file is part of Dilay
 * Copyright © 2015-2018 Alexander Bau
 * Use and redistribute under the terms of the GNU General Public License
 */
#include "sketch/mesh.hpp"
#include "sketch/node-intersection.hpp"

SketchNodeIntersection::SketchNodeIntersection ()
  : _node (nullptr)
{
}

bool SketchNodeIntersection::update (float d, const glm::vec3& p, const glm::vec3& n,
                                     SketchMesh& mesh, SketchNode& node)
{
  if (this->SketchMeshIntersection::update (d, p, n, mesh))
  {
    this->_node = &node;
    return true;
  }
  else
  {
    return false;
  }
}

SketchNode& SketchNodeIntersection::node () const
{
  assert (this->isIntersection ());
  assert (this->_node);
  return *this->_node;
}

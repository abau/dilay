/* This file is part of Dilay
 * Copyright © 2015-2017 Alexander Bau
 * Use and redistribute under the terms of the GNU General Public License
 */
#include <sstream>
#include "primitive/cone.hpp"
#include "primitive/cylinder.hpp"
#include "util.hpp"

PrimCylinder::PrimCylinder (const glm::vec3& c1, const glm::vec3& c2, float r, float l)
  : _center1 (c1)
  , _center2 (c2)
  , _radius (r)
  , _length (l)
  , _direction ((c2 - c1) / glm::vec3 (l))
{
}

PrimCylinder::PrimCylinder (const glm::vec3& c1, const glm::vec3& c2, float r)
  : PrimCylinder (c1, c2, r, glm::distance (c1, c2))
{
}

PrimCylinder::PrimCylinder (const PrimCone& cone)
  : _center1 (cone.center1 ())
  , _center2 (cone.center2 ())
  , _radius (cone.radius1 ())
  , _length (cone.length ())
  , _direction (cone.direction ())
{
  assert (cone.isCylinder ());
}

std::ostream& operator<< (std::ostream& os, const PrimCylinder& cylinder)
{
  os << "PrimCylinder { center1 = " << (cylinder.center1 ())
     << ", center2 = " << (cylinder.center2 ()) << ", radius = " << (cylinder.radius ()) << " }";
  return os;
}

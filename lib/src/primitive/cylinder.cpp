/* This file is part of Dilay
 * Copyright © 2015 Alexander Bau
 * Use and redistribute under the terms of the GNU General Public License
 */
#include <sstream>
#include "primitive/cylinder.hpp"
#include "util.hpp"

PrimCylinder :: PrimCylinder (const glm::vec3& c1, const glm::vec3& c2, float r) 
  : _center1   (c1)
  , _center2   (c2)
  , _radius    (r) 
  , _direction (glm::normalize (c2 - c1))
{}

std::ostream& operator<<(std::ostream& os, const PrimCylinder& cylinder) {
  os << "PrimCylinder { center1 = " << (cylinder.center1 ()) 
                  << ", center2 = " << (cylinder.center2 ()) 
                  << ", radius = "  << (cylinder.radius ()) << " }";
  return os;
}

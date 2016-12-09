/* This file is part of Dilay
 * Copyright © 2015,2016 Alexander Bau
 * Use and redistribute under the terms of the GNU General Public License
 */
#include <glm/gtx/norm.hpp>
#include <sstream>
#include "primitive/sphere.hpp"
#include "util.hpp"

PrimSphere :: PrimSphere (const glm::vec3& o, float r) 
  : _center (o)
  , _radius (r) 
{}

bool PrimSphere :: contains (const glm::vec3& p) const {
  return glm::distance2 (this->_center, p) <= (this->_radius * this->_radius);
}

std::ostream& operator<<(std::ostream& os, const PrimSphere& sphere) {
  os << "PrimSphere { center = " << (sphere.center ()) 
                << ", radius = " << (sphere.radius ()) << " }";
  return os;
}

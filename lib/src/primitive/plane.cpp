/* This file is part of Dilay
 * Copyright © 2015,2016 Alexander Bau
 * Use and redistribute under the terms of the GNU General Public License
 */
#include <sstream>
#include "primitive/plane.hpp"
#include "util.hpp"

PrimPlane::PrimPlane (const glm::vec3& p, const glm::vec3& n)
  : _point (p)
  , _normal (glm::normalize (n))
{
}

float PrimPlane::distance (const glm::vec3& p) const
{
  return glm::dot (this->_normal, p - this->_point);
}

float PrimPlane::absDistance (const glm::vec3& p) const
{
  return glm::abs (this->distance (p));
}

bool PrimPlane::onPlane (const glm::vec3& p) const
{
  return this->absDistance (p) < Util::epsilon ();
}

glm::vec3 PrimPlane::project (const glm::vec3& p) const
{
  return p - (this->_normal * this->distance (p));
}

glm::vec3 PrimPlane::projectDirection (const glm::vec3& d) const
{
  return this->project (d + this->_point) - this->_point;
}

glm::vec3 PrimPlane::mirror (const glm::vec3& p) const
{
  return p - (2.0f * this->_normal * this->distance (p));
}

glm::vec3 PrimPlane::mirrorDirection (const glm::vec3& d) const
{
  return this->mirror (d + this->_point) - this->_point;
}

std::ostream& operator<< (std::ostream& os, const PrimPlane& plane)
{
  os << "PrimPlane { point = " << (plane.point ()) << ", normal = " << (plane.normal ()) << " }";
  return os;
}

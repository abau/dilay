/* This file is part of Dilay
 * Copyright © 2015,2016 Alexander Bau
 * Use and redistribute under the terms of the GNU General Public License
 */
#include <sstream>
#include "primitive/ray.hpp"
#include "util.hpp"

PrimRay::PrimRay (bool l, const glm::vec3& o, const glm::vec3& d)
  : _isLine (l)
  , _origin (o)
  , _direction (glm::normalize (d))
{
}

PrimRay::PrimRay (const glm::vec3& o, const glm::vec3& d)
  : PrimRay (false, o, d)
{
}

glm::vec3 PrimRay::pointAt (float t) const
{
  assert (t >= 0.0f || this->_isLine);
  return this->_origin + (this->_direction * glm::vec3 (t));
}

float PrimRay::distance (const glm::vec3& p) const
{
  const float t = glm::dot (this->_direction, p - this->_origin);

  return t >= 0.0f || this->_isLine ? glm::distance (p, this->pointAt (t))
                                    : glm::distance (p, this->_origin);
}

bool PrimRay::onRay (const glm::vec3& p) const
{
  return this->distance (p) < Util::epsilon ();
}

std::ostream& operator<< (std::ostream& os, const PrimRay& ray)
{
  os << "PrimRay { origin = " << (ray.origin ()) << ", direction = " << (ray.direction ()) << " }";
  return os;
}

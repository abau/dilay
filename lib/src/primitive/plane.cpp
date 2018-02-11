/* This file is part of Dilay
 * Copyright © 2015-2018 Alexander Bau
 * Use and redistribute under the terms of the GNU General Public License
 */
#include "primitive/plane.hpp"
#include "util.hpp"

PrimPlane::PrimPlane (const glm::vec3& p, const glm::vec3& n)
  : _point (p)
{
  this->normal (n);
}

void PrimPlane::normal (const glm::vec3& n)
{
  this->_normal = glm::normalize (n);
  this->_u = glm::normalize (Util::orthogonal (this->_normal));
  this->_v = glm::normalize (glm::cross (this->_normal, this->_u));
}

float PrimPlane::distance (const glm::vec3& p) const
{
  return glm::dot (this->_normal, p - this->_point);
}

float PrimPlane::absDistance (const glm::vec3& p) const { return glm::abs (this->distance (p)); }

bool PrimPlane::onPlane (const glm::vec3& p) const
{
  return this->absDistance (p) < Util::epsilon ();
}

glm::vec3 PrimPlane::project (const glm::vec3& p) const
{
  return p - (this->_normal * this->distance (p));
}

glm::vec3 PrimPlane::project (const glm::vec2& p) const
{
  return this->_point + (this->_u * p.x) + (this->_v * p.y);
}

glm::vec2 PrimPlane::project2d (const glm::vec3& p) const
{
  const glm::vec3 proj = this->project (p) - this->_point;

  return glm::vec2 (glm::dot (this->_u, proj), glm::dot (this->_v, proj));
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

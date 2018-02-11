/* This file is part of Dilay
 * Copyright © 2015-2018 Alexander Bau
 * Use and redistribute under the terms of the GNU General Public License
 */
#include <glm/gtx/norm.hpp>
#include "primitive/triangle.hpp"
#include "util.hpp"

PrimTriangle::PrimTriangle (const glm::vec3& v1, const glm::vec3& v2, const glm::vec3& v3)
  : _vertex1 (v1)
  , _vertex2 (v2)
  , _vertex3 (v3)
{
}

glm::vec3 PrimTriangle::cross () const
{
  return glm::cross (this->_vertex2 - this->_vertex1, this->_vertex3 - this->_vertex1);
}

glm::vec3 PrimTriangle::normal () const
{
  const float l = glm::length (this->cross ());

  return l > 0.0f ? (this->cross () / l) : glm::vec3 (0.0f);
}

glm::vec3 PrimTriangle::center () const
{
  return (this->_vertex1 + this->_vertex2 + this->_vertex3) / glm::vec3 (3.0f);
}

glm::vec3 PrimTriangle::minimum () const
{
  return glm::min (glm::min (this->_vertex1, this->_vertex2), this->_vertex3);
}

glm::vec3 PrimTriangle::maximum () const
{
  return glm::max (glm::max (this->_vertex1, this->_vertex2), this->_vertex3);
}

float PrimTriangle::maxExtent () const { return glm::length (this->maximum () - this->minimum ()); }

float PrimTriangle::maxDimExtent () const
{
  const glm::vec3 extent = this->maximum () - this->minimum ();
  return glm::max (glm::max (extent.x, extent.y), extent.z);
}

float PrimTriangle::incircleRadiusSqr () const
{
  const float a = glm::distance (this->_vertex1, this->_vertex2);
  const float b = glm::distance (this->_vertex2, this->_vertex3);
  const float c = glm::distance (this->_vertex1, this->_vertex3);
  const float s = 0.5f * (a + b + c);
  return (s - a) * (s - b) * (s - c) / s;
}

float PrimTriangle::longestEdgeSqr () const
{
  return glm::max (glm::max (glm::distance2 (this->_vertex1, this->_vertex2),
                             glm::distance2 (this->_vertex2, this->_vertex3)),
                   glm::distance2 (this->_vertex1, this->_vertex3));
}

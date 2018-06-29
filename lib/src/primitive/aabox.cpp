/* This file is part of Dilay
 * Copyright © 2015-2018 Alexander Bau
 * Use and redistribute under the terms of the GNU General Public License
 */
#include "primitive/aabox.hpp"

PrimAABox::PrimAABox (const glm::vec3& min, const glm::vec3& max)
  : _minimum (min)
  , _maximum (max)
  , _center ((min + max) * 0.5f)
{
}

PrimAABox::PrimAABox (const glm::vec3& pos, float xW, float yW, float zW)
  : PrimAABox (pos - glm::vec3 (xW * 0.5f, yW * 0.5f, zW * 0.5f),
               pos + glm::vec3 (xW * 0.5f, yW * 0.5f, zW * 0.5f))
{
}

PrimAABox::PrimAABox (const glm::vec3& pos, float w)
  : PrimAABox (pos, w, w, w)
{
}

glm::vec3 PrimAABox::halfWidth () const { return (this->_maximum - this->_minimum) * 0.5f; }

float PrimAABox::maxDimExtent () const
{
  const glm::vec3 extent = this->_maximum - this->_minimum;
  return glm::max (glm::max (extent.x, extent.y), extent.z);
}

bool PrimAABox::contains (const PrimAABox& box) const
{
  return glm::all (glm::lessThanEqual (this->_minimum, box._minimum)) &&
         glm::all (glm::greaterThanEqual (this->_maximum, box._maximum));
}

/* This file is part of Dilay
 * Copyright © 2015,2016 Alexander Bau
 * Use and redistribute under the terms of the GNU General Public License
 */
#include "primitive/aabox.hpp"

PrimAABox :: PrimAABox (const glm::vec3& min, const glm::vec3& max) 
  : _minimum (min)
  , _maximum (max)
  , _center  ((min + max) * 0.5f)
{}

PrimAABox :: PrimAABox (const glm::vec3& pos, float xW, float yW, float zW)
  : PrimAABox ( pos - glm::vec3 (xW * 0.5f, yW * 0.5f, zW * 0.5f)
              , pos + glm::vec3 (xW * 0.5f, yW * 0.5f, zW * 0.5f))
{}

PrimAABox :: PrimAABox (const glm::vec3& pos, float w) 
  : PrimAABox (pos, w, w, w) 
{}

float PrimAABox :: xWidth () const { return this->_maximum.x - this->_minimum.x; }
float PrimAABox :: yWidth () const { return this->_maximum.y - this->_minimum.y; }
float PrimAABox :: zWidth () const { return this->_maximum.z - this->_minimum.z; }


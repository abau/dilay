#include "primitive/aabox.hpp"

PrimAABox :: PrimAABox (const glm::vec3& min, const glm::vec3& max) 
  : _maximum (min)
  , _minimum (max)
  , _center  ((min + max) * 0.5f)
{}

PrimAABox :: PrimAABox (const glm::vec3& pos, float xW, float yW, float zW)
  : PrimAABox ( pos + glm::vec3 (xW * 0.5f, yW * 0.5f, zW * 0.5f)
              , pos - glm::vec3 (xW * 0.5f, yW * 0.5f, zW * 0.5f))
{}

PrimAABox :: PrimAABox (const glm::vec3& pos, float w) 
  : PrimAABox (pos, w, w, w) 
{}

float PrimAABox :: xWidth () const { return this->_maximum.x - this->_minimum.x; }
float PrimAABox :: yWidth () const { return this->_maximum.y - this->_minimum.y; }
float PrimAABox :: zWidth () const { return this->_maximum.z - this->_minimum.z; }


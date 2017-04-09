/* This file is part of Dilay
 * Copyright © 2015,2016 Alexander Bau
 * Use and redistribute under the terms of the GNU General Public License
 */
#ifndef DILAY_PRIMITIVE_CYLINDER
#define DILAY_PRIMITIVE_CYLINDER

#include <glm/glm.hpp>
#include <iosfwd>

class PrimCone;

class PrimCylinder
{
public:
  PrimCylinder (const glm::vec3&, const glm::vec3&, float);
  PrimCylinder (const glm::vec3&, const glm::vec3&, float, float);
  PrimCylinder (const PrimCone&);

  const glm::vec3& center1 () const
  {
    return this->_center1;
  }
  const glm::vec3& center2 () const
  {
    return this->_center2;
  }
  float radius () const
  {
    return this->_radius;
  }
  float length () const
  {
    return this->_length;
  }
  const glm::vec3& direction () const
  {
    return this->_direction;
  }

private:
  const glm::vec3 _center1;
  const glm::vec3 _center2;
  const float     _radius;
  const float     _length;
  const glm::vec3 _direction;
};

std::ostream& operator<< (std::ostream&, const PrimCylinder&);

#endif

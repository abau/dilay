/* This file is part of Dilay
 * Copyright © 2015-2017 Alexander Bau
 * Use and redistribute under the terms of the GNU General Public License
 */
#ifndef DILAY_PRIMITIVE_CONE
#define DILAY_PRIMITIVE_CONE

#include <glm/glm.hpp>
#include <iosfwd>

class PrimCone
{
public:
  PrimCone (const glm::vec3&, float, const glm::vec3&, float);
  PrimCone (const glm::vec3&, float, const glm::vec3&, float, float);

  const glm::vec3& center1 () const
  {
    return this->_center1;
  }

  float radius1 () const
  {
    return this->_radius1;
  }

  const glm::vec3& center2 () const
  {
    return this->_center2;
  }

  float radius2 () const
  {
    return this->_radius2;
  }

  float length () const
  {
    return this->_length;
  }

  const glm::vec3& direction () const
  {
    return this->_direction;
  }

  bool isCylinder () const
  {
    return this->_isCylinder;
  }

  const glm::vec3& apex () const
  {
    return this->_apex;
  }

  float alpha () const
  {
    return this->_alpha;
  }

  float sinAlpha () const
  {
    return this->_sinAlpha;
  }

  float cosAlpha () const
  {
    return this->_cosAlpha;
  }

  glm::vec3 projPointAt (float) const;
  glm::vec3 normalAt (const glm::vec3&, float) const;

private:
  const glm::vec3 _center1;
  const float     _radius1;
  const glm::vec3 _center2;
  const float     _radius2;
  const float     _length;
  const glm::vec3 _direction;
  const bool      _isCylinder;
  const glm::vec3 _apex;
  const float     _alpha;
  const float     _sinAlpha;
  const float     _cosAlpha;
};

std::ostream& operator<< (std::ostream&, const PrimCone&);

#endif

/* This file is part of Dilay
 * Copyright © 2015-2017 Alexander Bau
 * Use and redistribute under the terms of the GNU General Public License
 */
#ifndef DILAY_PRIMITIVE_RAY
#define DILAY_PRIMITIVE_RAY

#include <glm/glm.hpp>

class PrimRay
{
public:
  PrimRay (bool, const glm::vec3&, const glm::vec3&);
  PrimRay (const glm::vec3&, const glm::vec3&);

  bool             isLine () const { return this->_isLine; }
  const glm::vec3& origin () const { return this->_origin; }
  const glm::vec3& direction () const { return this->_direction; }

  glm::vec3 pointAt (float) const;
  float     distance (const glm::vec3&) const;
  bool      onRay (const glm::vec3&) const;

private:
  const bool      _isLine;
  const glm::vec3 _origin;
  const glm::vec3 _direction;
};

#endif

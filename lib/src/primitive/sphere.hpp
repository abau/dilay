/* This file is part of Dilay
 * Copyright © 2015-2018 Alexander Bau
 * Use and redistribute under the terms of the GNU General Public License
 */
#ifndef DILAY_PRIMITIVE_SPHERE
#define DILAY_PRIMITIVE_SPHERE

#include <glm/glm.hpp>

class PrimAABox;
class PrimTriangle;

class PrimSphere
{
public:
  PrimSphere (const glm::vec3&, float);

  const glm::vec3& center () const { return this->_center; }
  float            radius () const { return this->_radius; }

  void center (const glm::vec3& c) { this->_center = c; }
  void radius (float r) { this->_radius = r; }

  bool contains (const glm::vec3&) const;
  bool contains (const PrimAABox&) const;
  bool contains (const PrimTriangle&) const;

private:
  glm::vec3 _center;
  float     _radius;
};

#endif

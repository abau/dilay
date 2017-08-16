/* This file is part of Dilay
 * Copyright © 2015-2017 Alexander Bau
 * Use and redistribute under the terms of the GNU General Public License
 */
#ifndef DILAY_PRIMITIVE_PLANE
#define DILAY_PRIMITIVE_PLANE

#include <glm/glm.hpp>
#include <iosfwd>
#include "macro.hpp"

class PrimPlane
{
public:
  PrimPlane (const glm::vec3&, const glm::vec3&);

  float     distance (const glm::vec3&) const;
  float     absDistance (const glm::vec3&) const;
  bool      onPlane (const glm::vec3&) const;
  glm::vec3 project (const glm::vec3&) const;
  glm::vec3 project (const glm::vec2&) const;
  glm::vec2 project2d (const glm::vec3&) const;
  glm::vec3 projectDirection (const glm::vec3&) const;
  glm::vec3 mirror (const glm::vec3&) const;
  glm::vec3 mirrorDirection (const glm::vec3&) const;

  MEMBER_REF_GETTER_SETTER (glm::vec3, point)
  MEMBER_REF_GETTER_SETTER (glm::vec3, normal)
};

std::ostream& operator<< (std::ostream&, const PrimPlane&);

#endif

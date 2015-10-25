/* This file is part of Dilay
 * Copyright © 2015 Alexander Bau
 * Use and redistribute under the terms of the GNU General Public License
 */
#ifndef DILAY_DISTANCE
#define DILAY_DISTANCE

#include <glm/fwd.hpp>

class PrimCone;
class PrimConeSphere;
class PrimCylinder;
class PrimSphere;

namespace Distance {
  float distance (const PrimSphere&, const glm::vec3&);
  float distance (const PrimCylinder&, const glm::vec3&);
  float distance (const PrimCone&, const glm::vec3&);
  float distance (const PrimConeSphere&, const glm::vec3&);
}

#endif

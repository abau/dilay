/* This file is part of Dilay
 * Copyright © 2015 Alexander Bau
 * Use and redistribute under the terms of the GNU General Public License
 */
#ifndef DILAY_DIMENSION
#define DILAY_DIMENSION

#include <glm/fwd.hpp>

enum class Dimension { X, Y, Z };

namespace DimensionUtil {
  unsigned int index  (Dimension);
  glm::vec3    vector (Dimension);
}

#endif

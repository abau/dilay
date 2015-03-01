#ifndef DILAY_DIMENSION
#define DILAY_DIMENSION

#include <glm/fwd.hpp>

enum class Dimension { X, Y, Z };

namespace DimensionUtil {
  unsigned int index       (Dimension);
  glm::vec3    vector      (Dimension);
  glm::vec3    orthVector1 (Dimension);
  glm::vec3    orthVector2 (Dimension);
}

#endif

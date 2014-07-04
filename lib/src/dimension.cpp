#include <cassert>
#include <glm/glm.hpp>
#include "dimension.hpp"

unsigned int DimensionUtil::index (Dimension d) {
  switch (d) {
    case Dimension::X: return 0;
    case Dimension::Y: return 1;
    case Dimension::Z: return 2;
  }
  assert (false);
}

glm::vec3 DimensionUtil::vector (Dimension d) {
  switch (d) {
    case Dimension::X: return glm::vec3 (1.0f, 0.0f, 0.0f);
    case Dimension::Y: return glm::vec3 (0.0f, 1.0f, 0.0f);
    case Dimension::Z: return glm::vec3 (0.0f, 0.0f, 1.0f);
  }
  assert (false);
}

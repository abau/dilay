/* This file is part of Dilay
 * Copyright © 2015,2016 Alexander Bau
 * Use and redistribute under the terms of the GNU General Public License
 */
#include <cstdlib>
#include <glm/glm.hpp>
#include "dimension.hpp"
#include "util.hpp"

unsigned int DimensionUtil::index (Dimension d) {
  switch (d) {
    case Dimension::X: return 0;
    case Dimension::Y: return 1;
    case Dimension::Z: return 2;
  }
  DILAY_IMPOSSIBLE
}

glm::vec3 DimensionUtil::vector (Dimension d) {
  switch (d) {
    case Dimension::X: return glm::vec3 (1.0f, 0.0f, 0.0f);
    case Dimension::Y: return glm::vec3 (0.0f, 1.0f, 0.0f);
    case Dimension::Z: return glm::vec3 (0.0f, 0.0f, 1.0f);
  }
  DILAY_IMPOSSIBLE
}

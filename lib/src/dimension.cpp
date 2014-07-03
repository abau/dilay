#include <cassert>
#include "dimension.hpp"

unsigned int DimensionUtil::index (Dimension d) {
  switch (d) {
    case Dimension::X: return 0;
    case Dimension::Y: return 1;
    case Dimension::Z: return 2;
  }
  assert (false);
}

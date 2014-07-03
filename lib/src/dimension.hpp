#ifndef DILAY_DIMENSION
#define DILAY_DIMENSION

enum class Dimension { X, Y, Z };

namespace DimensionUtil {
  unsigned int index (Dimension);
}

#endif

#ifndef DILAY_CARVE_BRUSH
#define DILAY_CARVE_BRUSH

#include "macro.hpp"

class CarveBrush {
  public:
    DECLARE_BIG6 (CarveBrush, float, float)

    float y      (float) const;
    float width  ()      const;
    float height ()      const;
  private:
    class Impl;
    Impl* impl;
};

#endif

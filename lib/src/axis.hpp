#ifndef DILAY_AXIS
#define DILAY_AXIS

#include "macro.hpp"

class QPainter;

class Axis {
  public:
    DECLARE_BIG3 (Axis)

    void  initialize ();
    void  render     ();
    void  render     (QPainter&);

  private:
    IMPLEMENTATION
};

#endif

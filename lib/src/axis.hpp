#ifndef DILAY_AXIS
#define DILAY_AXIS

#include "macro.hpp"

class Axis {
  public:
    DECLARE_BIG3 (Axis)

    void  initialize ();
    void  render     ();

  private:
    class Impl;
    Impl* impl;
};

#endif

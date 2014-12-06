#ifndef DILAY_VIEW_AXIS
#define DILAY_VIEW_AXIS

#include "macro.hpp"

class QPainter;

class ViewAxis {
  public:
    DECLARE_BIG3 (ViewAxis)

    void  initialize ();
    void  render     ();
    void  render     (QPainter&);

  private:
    IMPLEMENTATION
};

#endif

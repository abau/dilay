#ifndef DILAY_VIEW_AXIS
#define DILAY_VIEW_AXIS

#include "configurable.hpp"
#include "macro.hpp"

class Camera;
class Config;
class QPainter;

class ViewAxis : public Configurable {
  public:
    DECLARE_BIG3 (ViewAxis, const Config&)

    void  render (Camera&);
    void  render (Camera&, QPainter&);

  private:
    IMPLEMENTATION

    void runFromConfig (const Config&);
};

#endif

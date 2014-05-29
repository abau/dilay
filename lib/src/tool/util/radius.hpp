#ifndef DILAY_TOOL_UTIL_RADIUS
#define DILAY_TOOL_UTIL_RADIUS

#include "tool/util.hpp"

class ConfigProxy;
class ToolMovement;
class QMouseEvent;
class QDoubleSpinBox;

class ToolUtilRadius : public ToolUtil {
  public:
    DECLARE_BIG3 (ToolUtilRadius, Tool&, const ConfigProxy&, const ToolMovement&)

    const QDoubleSpinBox& radiusEdit        () const;
    float                 radius            () const;
    void                  radius            (float);
    bool                  runMouseMoveEvent (QMouseEvent& e);

  private:
    class Impl;
    Impl* impl;
};

#endif

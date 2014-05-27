#ifndef DILAY_TOOL_UTIL_POSITION
#define DILAY_TOOL_UTIL_POSITION

#include "tool/util.hpp"

class ToolUtilPosition : public ToolUtil {
  public:
    DECLARE_BIG3 (ToolUtilPosition,Tool&)

  private:
    class Impl;
    Impl* impl;
};

#endif

#ifndef DILAY_TOOL_UTIL_RADIUS
#define DILAY_TOOL_UTIL_RADIUS

#include "tool/util.hpp"

class ToolUtilInteger : public ToolUtil {
  public:
    DECLARE_BIG3 (ToolUtilInteger, Tool&, const std::string&, int, int, int)

    int  value    () const;
    void increase ();
    void decrease ();

  private:
    class Impl;
    Impl* impl;
};

#endif

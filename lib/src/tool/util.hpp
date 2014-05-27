#ifndef DILAY_TOOL_UTIL
#define DILAY_TOOL_UTIL

#include "macro.hpp"

class Tool;
class ViewToolOptions;

class ToolUtil {
  public:
    DECLARE_BIG3_VIRTUAL (ToolUtil, Tool&)

  protected:
    Tool&            tool        ();
    ViewToolOptions* toolOptions ();

  private:
    class Impl;
    Impl* impl;
};

#endif

#include "tool/util.hpp"
#include "tool.hpp"
#include "view/tool-options.hpp"

struct ToolUtil::Impl {
  Tool& tool;

  Impl (Tool& t) : tool (t) { }

  ViewToolOptions* toolOptions () { return this->tool.toolOptions (); }

};

DELEGATE1_BIG3 (ToolUtil, Tool&)
GETTER         (Tool&, ToolUtil, tool)
DELEGATE       (ViewToolOptions*, ToolUtil, toolOptions)

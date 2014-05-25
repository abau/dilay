#include "tool/util/position.hpp"

struct ToolUtilPosition::Impl {
  ToolUtilPosition* self;

  Impl (ToolUtilPosition* s) : self (s) {}
};

DELEGATE_TOOL_UTIL (ToolUtilPosition)

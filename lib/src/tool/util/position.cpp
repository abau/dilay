#include <utility>
#include "tool/util/position.hpp"

struct ToolUtilPosition::Impl {
  ToolUtilPosition* self;

  Impl (ToolUtilPosition* s) : self (s) {}
};

DELEGATE_BIG3_BASE (ToolUtilPosition,(Tool& t),(this),ToolUtil,(t))

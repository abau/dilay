#include <cassert>
#include "selection-mode.hpp"

bool SelectionModeUtil::isMajor (SelectionMode mode) {
  switch (mode) {
    case SelectionMode::Freeform  : return true;
    case SelectionMode::SphereNode: return false;
    default:
      assert (false);
  }
}

bool SelectionModeUtil::isMinor (SelectionMode mode) {
  return ! SelectionModeUtil::isMajor (mode);
}

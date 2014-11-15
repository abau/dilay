#include <cassert>
#include <cstdlib>
#include "selection-mode.hpp"

bool SelectionModeUtil::isMajor (SelectionMode mode) {
  switch (mode) {
    case SelectionMode::Freeform  : return true;
  }
  std::abort ();
}

bool SelectionModeUtil::isMinor (SelectionMode mode) {
  return ! SelectionModeUtil::isMajor (mode);
}

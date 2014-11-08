#include <cassert>
#include <cstdlib>
#include "selection-mode.hpp"

bool SelectionModeUtil::isMajor (SelectionMode mode) {
  switch (mode) {
    case SelectionMode::Freeform  : return true;
    case SelectionMode::Sphere    : return true;
    case SelectionMode::SphereNode: return false;
  }
  std::abort ();
}

bool SelectionModeUtil::isMinor (SelectionMode mode) {
  return ! SelectionModeUtil::isMajor (mode);
}

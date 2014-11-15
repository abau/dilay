#ifndef DILAY_SELECTION_MODE
#define DILAY_SELECTION_MODE

enum class SelectionMode {
  Freeform
};

namespace SelectionModeUtil {

  bool isMajor (SelectionMode);
  bool isMinor (SelectionMode);

};

#endif

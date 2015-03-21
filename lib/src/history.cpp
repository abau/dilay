#include <cstdlib>
#include "history.hpp"

struct History::Impl {

  void undo () {
    std::abort ();
  }

  void redo () {
    std::abort ();
  }
};

DELEGATE_BIG3 (History)
DELEGATE (void, History, undo)
DELEGATE (void, History, redo)

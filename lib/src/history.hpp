#ifndef DILAY_HISTORY
#define DILAY_HISTORY

#include "macro.hpp"

class History {
  public: 
    DECLARE_BIG3 (History)

    void undo ();
    void redo ();

  private:
    IMPLEMENTATION
};

#endif

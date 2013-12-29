#ifndef DILAY_ACTION_ID_STATE
#define DILAY_ACTION_ID_STATE

#include "macro.hpp"
#include "action.hpp"

class ActionIdState : public Action {
  public: 
    DECLARE_ACTION_BIG5 (ActionIdState)

    void run  ();
    void undo ();
    void redo ();

  private:
    class Impl;
    Impl* impl;
};

#endif

#ifndef DILAY_ACTION_ID_STATE
#define DILAY_ACTION_ID_STATE

#include "action.hpp"

class ActionIdState : public Action {
  public: ActionIdState            ();
          ActionIdState            (const ActionIdState&) = delete;
    const ActionIdState& operator= (const ActionIdState&) = delete;
         ~ActionIdState            ();

    void run  ();
    void undo ();
    void redo ();

  private:
    class Impl;
    Impl* impl;
};

#endif

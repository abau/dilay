#ifndef DILAY_ACTION_UNIT
#define DILAY_ACTION_UNIT

#include "action.hpp"

class ActionUnit : public Action {
  public: ActionUnit            ();
          ActionUnit            (const ActionUnit&) = delete;
    const ActionUnit& operator= (const ActionUnit&) = delete;
         ~ActionUnit            ();

    template <class T>
    void addAction (const T& action) { this->addActionPtr (new T (action)); }

    void undo ();
    void redo ();
  private:
    void addActionPtr (Action*);

    class Impl;
    Impl* impl;
};
#endif

#ifndef DILAY_ACTION_UNIT
#define DILAY_ACTION_UNIT

#include "action.hpp"
#include "action/sub-action-kind.hpp"
#include "macro.hpp"

class ActionUnit : public Action {
  public: 
    DECLARE_ACTION_BIG6 (ActionUnit)

    template <class T>
    T* add (SubActionKind kind = SubActionKind::Normal) { 
      T* action = new T ();
      this->addAction (action, kind); 
      return action; 
    }

    void addAction (Action*, SubActionKind);
    void reset     ();
    void undo      ();
    void redo      ();
  private:
    class Impl;
    Impl* impl;
};
#endif

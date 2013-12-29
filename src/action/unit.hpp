#ifndef DILAY_ACTION_UNIT
#define DILAY_ACTION_UNIT

#include "action.hpp"
#include "macro.hpp"

class ActionUnit : public Action {
  public: 
    DECLARE_ACTION_BIG5 (ActionUnit)

    template <class T>
    T* add () { 
      T* action = new T ();
      this->addAction (action); 
      return action; 
    }

    void addAction (Action*);
    void reset     ();
    void undo      ();
    void redo      ();
  private:
    class Impl;
    Impl* impl;
};
#endif

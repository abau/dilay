#ifndef DILAY_ACTION_UNIT
#define DILAY_ACTION_UNIT

#include "action.hpp"
#include "macro.hpp"

class ActionUnit : public Action {
  public: 
    DECLARE_ACTION_BIG6 (ActionUnit)

    template <class T>
    T* add () { 
      T* action = new T ();
      this->addAction (action); 
      return action; 
    }

    void         addAction  (Action*);
    void         reset      ();
    unsigned int numActions () const;

  private:
    void runUndo ();
    void runRedo ();

    class Impl;
    Impl* impl;
};
#endif

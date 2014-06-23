#ifndef DILAY_ACTION_UNIT
#define DILAY_ACTION_UNIT

#include "action.hpp"
#include "macro.hpp"

class ActionUnit : public Action {
  public: 
    DECLARE_BIG3 (ActionUnit)

    template <class T>
    T& add () { 
      T& action = *new T ();
      this->addAction (action); 
      return action; 
    }

    bool isEmpty () const;

  private:
    void addAction (Action&);
    void runUndo   ();
    void runRedo   ();

    class Impl;
    Impl* impl;
};
#endif

#ifndef DILAY_ACTION_UNIT
#define DILAY_ACTION_UNIT

#include "action.hpp"
#include "action/transformer.hpp"
#include "macro.hpp"

class ActionUnit : public Action {
  public: 
    DECLARE_BIG3 (ActionUnit)

    template <class A>
    A& add () { 
      A& action = *new A ();
      this->addAction (action); 
      return action; 
    }

    template <typename A, typename T>
    A& add (T& t) { 
      A& action = *new A ();
      this->addAction (*new ActionTransformer <T> (t, action));
      return *action; 
    }

    bool isEmpty () const;

  private:
    void addAction (Action&);
    void runUndo   ();
    void runRedo   ();

    IMPLEMENTATION
};
#endif

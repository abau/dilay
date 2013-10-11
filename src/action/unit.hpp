#ifndef DILAY_ACTION_UNIT
#define DILAY_ACTION_UNIT

#include "action.hpp"

class ActionUnit : public Action {
  public: ActionUnit            ();
          ActionUnit            (const ActionUnit&) = delete;
    const ActionUnit& operator= (const ActionUnit&) = delete;
         ~ActionUnit            ();

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

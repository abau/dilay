#ifndef DILAY_ACTION_UNIT_ON
#define DILAY_ACTION_UNIT_ON

#include <list>
#include <memory>
#include <functional>

template <typename T>
class ActionUnitOn : public ActionOn <T> {
  public: 
          ActionUnitOn () 
            : actions () {}
          ActionUnitOn (ActionUnitOn&& other) 
            : actions (std::move (other.actions)) {}
          ActionUnitOn (const ActionUnitOn&) = delete;
         ~ActionUnitOn () {}

    const ActionUnitOn& operator= (const ActionUnitOn&) = delete;
    const ActionUnitOn& operator= (ActionUnitOn&&)      = delete;

    typedef std::unique_ptr < ActionOn <T> > ActionPtr;

    template <typename A>
    A& add () {
      A& action = *new A ();
      this->actions.push_back (ActionPtr (&action));
      return action;
    }

    bool isEmpty () const { return this->actions.empty (); }

  private:

    void runUndo (T& t) {
      for (auto it = this->actions.rbegin (); it != this->actions.rend (); ++it) {
        (*it)->undo (t);
      }
    }

    void runRedo (T& t) {
      for (auto it = this->actions.begin (); it != this->actions.end (); ++it) {
        (*it)->redo (t);
      }
    }

    std::list < ActionPtr > actions;
};
#endif

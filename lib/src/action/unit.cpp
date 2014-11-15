#include <list>
#include <memory>
#include "action/unit.hpp"

namespace {
  typedef std::unique_ptr <Action> ActionPtr;
};

struct ActionUnit :: Impl {
  std::list <ActionPtr> actions;

  bool isEmpty () const { return this->actions.empty (); }

  void addAction (Action& action) {
    this->actions.push_back (ActionPtr (&action));
  }

  void runUndo () const {
    for (auto it = this->actions.rbegin (); it != this->actions.rend (); ++it) {
      (*it)->undo ();
    }
  }

  void runRedo () const {
    for (auto it = this->actions.begin (); it != this->actions.end (); ++it) {
      (*it)->redo ();
    }
  }
};

DELEGATE_BIG3  (ActionUnit)

DELEGATE_CONST (bool, ActionUnit, isEmpty)
DELEGATE1      (void, ActionUnit, addAction, Action&)
DELEGATE_CONST (void, ActionUnit, runUndo)
DELEGATE_CONST (void, ActionUnit, runRedo)

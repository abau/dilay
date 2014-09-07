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

  void runUndo () {
    for (auto it = this->actions.rbegin (); it != this->actions.rend (); ++it) {
      (*it)->undo ();
    }
  }

  void runRedo () {
    for (auto it = this->actions.begin (); it != this->actions.end (); ++it) {
      (*it)->redo ();
    }
  }
};

DELEGATE_BIG3  (ActionUnit)

DELEGATE_CONST (bool, ActionUnit, isEmpty)
DELEGATE1      (void, ActionUnit, addAction, Action&)
DELEGATE       (void, ActionUnit, runUndo)
DELEGATE       (void, ActionUnit, runRedo)

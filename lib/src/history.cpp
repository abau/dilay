#include <list>
#include <memory>
#include "action.hpp"
#include "action/unit.hpp"
#include "history.hpp"
#include "macro.hpp"

struct History::Impl {
  typedef std::unique_ptr <Action> ActionPtr;
  typedef std::list <ActionPtr>    Timeline;

  Timeline past;
  Timeline future;

  void addAction (Action* action) {
    this->future.clear ();
    this->past.push_front (ActionPtr (action));
  }

  void addUnit (ActionUnit&& unit) {
    this->addAction (new ActionUnit (std::move (unit)));
  }

  void undo () {
    if (! this->past.empty ()) {
      this->past.front ()->undo ();
      this->future.push_front   (ActionPtr (std::move(this->past.front ())));
      this->past.pop_front      ();
    }
  }

  void redo () {
    if (! this->future.empty ()) {
      this->future.front ()->redo ();
      this->past.push_front       (ActionPtr (std::move (this->future.front ())));
      this->future.pop_front      ();
    }
  }
};

DELEGATE_BIG3 (History)
DELEGATE1 (void, History, addAction, Action*)
DELEGATE1 (void, History, addUnit, ActionUnit&&)
DELEGATE  (void, History, undo)
DELEGATE  (void, History, redo)

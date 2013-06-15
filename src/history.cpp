#include <list>
#include <memory>
#include "history.hpp"
#include "macro.hpp"
#include "action.hpp"

typedef std::shared_ptr <Action> ActionPtr;
typedef std::list <ActionPtr>    Timeline;

struct History::Impl {
  Timeline past;
  Timeline future;

  void addAction (Action* action) {
    this->future.clear ();
    this->past.push_front (ActionPtr (action));
  }

  void undo () {
    if (! this->past.empty ()) {
      this->past.front ()->undo ();
      this->future.push_front   (ActionPtr (this->past.front ()));
      this->past.pop_front      ();
    }
  }

  void redo () {
    if (! this->future.empty ()) {
      this->future.front ()->redo ();
      this->past.push_front       (ActionPtr (this->future.front ()));
      this->future.pop_front      ();
    }
  }
};

DELEGATE_BIG4 (History)
DELEGATE1     (void, History, addAction, Action*)
DELEGATE      (void, History, undo)
DELEGATE      (void, History, redo)

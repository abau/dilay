#include <list>
#include <memory>
#include "history.hpp"
#include "macro.hpp"
#include "action.hpp"
#include "action/transformer.hpp"

typedef std::unique_ptr <Action> ActionPtr;
typedef std::list <ActionPtr>    Timeline;

struct History::Impl {
  Timeline past;
  Timeline future;

  void addAction (Action* action) {
    this->future.clear ();
    this->past.push_front (ActionPtr (action));
  }

  void addActionOnWMesh (WingedMesh& mesh, ActionOnWMesh* action) {
    this->addAction (new ActionTransformer (mesh, action));
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

DELEGATE_CONSTRUCTOR (History)
DELEGATE_DESTRUCTOR  (History)
DELEGATE1     (void, History, addAction, Action*)
DELEGATE2     (void, History, addActionOnWMesh, WingedMesh&, ActionOnWMesh*)
DELEGATE      (void, History, undo)
DELEGATE      (void, History, redo)

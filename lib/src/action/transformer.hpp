#ifndef DILAY_ACTION_TRANSFORMER
#define DILAY_ACTION_TRANSFORMER

#include <memory>
#include "action.hpp"
#include "scene.hpp"
#include "state.hpp"

template <typename T> class ActionOn;

template <typename T>
class ActionTransformer : public Action {
  public:
    ActionTransformer (T& t, ActionOn <T>& a) 
      : actionPtr (&a) 
      , index     (t.index ())
    {}

    ActionTransformer (ActionTransformer&&) = default;

    ActionTransformer (const ActionTransformer&) = delete;

    virtual ~ActionTransformer () {}

    const ActionTransformer& operator= (const ActionTransformer&) = delete;
    const ActionTransformer& operator= (ActionTransformer&&)      = delete;

  private:
    void runUndo () const { 
      this->actionPtr->undo (*State::scene ().template mesh <T> (this->index));
    }

    void runRedo () const {
      this->actionPtr->redo (*State::scene ().template mesh <T> (this->index));
    }

    std::unique_ptr <ActionOn <T>> actionPtr;
    const unsigned int             index;
};

#endif

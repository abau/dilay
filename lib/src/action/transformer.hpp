#ifndef DILAY_ACTION_TRANSFORMER
#define DILAY_ACTION_TRANSFORMER

#include <memory>
#include "action.hpp"
#include "action/ids.hpp"

template <typename T> class ActionOn;

template <typename T>
class ActionTransformer : public Action {
  public:
    ActionTransformer (T& t, ActionOn <T>& a) 
      : actionPtr (&a) 
    {
      this->operands.setMesh (0, t);
    }

    ActionTransformer (ActionTransformer&& other) 
      : actionPtr (std::move (other.actionPtr)) 
      , operands  (std::move (other.operands)) 
    {}

    ActionTransformer (const ActionTransformer&) = delete;

    virtual ~ActionTransformer () {}

    const ActionTransformer& operator= (const ActionTransformer&) = delete;
    const ActionTransformer& operator= (ActionTransformer&&)      = delete;

  private:
    void runUndo () { 
      this->actionPtr->undo (this->operands.template getMesh <T> (0));
    }

    void runRedo () {
      this->actionPtr->redo (this->operands.template getMesh <T> (0));
    }

    std::unique_ptr < ActionOn <T> > actionPtr;
    ActionIds                        operands;
};

#endif

#ifndef DILAY_ACTION_TRANSFORMER
#define DILAY_ACTION_TRANSFORMER

#include <memory>
#include "action.hpp"
#include "action/identifier.hpp"

template <typename T> class ActionOn;

template <typename T>
class ActionTransformer : public Action {
  public:
    ActionTransformer (T& t, ActionOn <T>& a) 
      : actionPtr (&a) 
    {
      this->identifier.setMesh (&t);
    }

    ActionTransformer (ActionTransformer&& other) 
      : actionPtr  (std::move (other.actionPtr)) 
      , identifier (std::move (other.identifier)) 
    {}

    ActionTransformer (const ActionTransformer&) = delete;

    virtual ~ActionTransformer () {}

    const ActionTransformer& operator= (const ActionTransformer&) = delete;
    const ActionTransformer& operator= (ActionTransformer&&)      = delete;

  private:
    void runUndo () const { 
      this->actionPtr->undo (*this->identifier.template getMesh <T> ());
    }

    void runRedo () const {
      this->actionPtr->redo (*this->identifier.template getMesh <T> ());
    }

    std::unique_ptr <ActionOn <T>> actionPtr;
    ActionIdentifier               identifier;
};

#endif

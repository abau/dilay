#ifndef DILAY_ACTION_TRANSFORMER
#define DILAY_ACTION_TRANSFORMER

#include <memory>
#include "action.hpp"

template <typename T> class ActionOn;
class Scene;

template <typename T>
class ActionTransformer : public Action {
  public:
    ActionTransformer (Scene& s, T& t, ActionOn <T>* a) 
      : scene     (s)
      , actionPtr (a) 
      , index     (t.index ())
    {}

    ActionTransformer (ActionTransformer&&) = default;

    ActionTransformer (const ActionTransformer&) = delete;

    virtual ~ActionTransformer () {}

    const ActionTransformer& operator= (const ActionTransformer&) = delete;
    const ActionTransformer& operator= (ActionTransformer&&)      = delete;

  private:
    void runUndo () const { 
      this->actionPtr->undo (*this->scene.template mesh <T> (this->index));
    }

    void runRedo () const {
      this->actionPtr->redo (*this->scene.template mesh <T> (this->index));
    }

    Scene&                         scene;
    std::unique_ptr <ActionOn <T>> actionPtr;
    const unsigned int             index;
};

#endif

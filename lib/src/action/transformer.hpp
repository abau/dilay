#ifndef DILAY_ACTION_TRANSFORMER
#define DILAY_ACTION_TRANSFORMER

#include <memory>
#include "action.hpp"

class ActionOnWMesh;
class Scene;
class WingedMesh;

class ActionTransformer : public Action {
  public:
          ActionTransformer            (Scene&, WingedMesh&, ActionOnWMesh&&);
          ActionTransformer            (      ActionTransformer&&) = default;
          ActionTransformer            (const ActionTransformer&)  = delete;
    const ActionTransformer& operator= (const ActionTransformer&)  = delete;
    const ActionTransformer& operator= (ActionTransformer&&)       = delete;

    virtual ~ActionTransformer ();

  private:
    void runUndo () const;
    void runRedo () const;

    Scene&                          scene;
    std::unique_ptr <ActionOnWMesh> actionPtr;
    const unsigned int              index;
};

#endif

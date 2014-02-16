#ifndef DILAY_ACTION_TRANSFORMER
#define DILAY_ACTION_TRANSFORMER

#include "macro.hpp"
#include "action.hpp"

class WingedMesh;
class ActionOnWMesh;

class ActionTransformer : public Action {
  public:
    DECLARE_BIG3 (ActionTransformer, WingedMesh&, ActionOnWMesh*);

  private:
    void runUndo ();
    void runRedo ();

    class Impl;
    Impl* impl;
};

#endif

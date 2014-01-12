#ifndef DILAY_ACTION_TRANSFORMER
#define DILAY_ACTION_TRANSFORMER

#include "macro.hpp"
#include "action.hpp"

class WingedMesh;
class ActionOnWMesh;

class ActionTransformer : public Action {
  public:
    DECLARE_ACTION_BIG6 (ActionTransformer, WingedMesh&, ActionOnWMesh*);

    void undo ();
    void redo ();

  private:
    class Impl;
    Impl* impl;
};

#endif

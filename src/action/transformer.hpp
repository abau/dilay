#ifndef DILAY_ACTION_TRANSFORMER
#define DILAY_ACTION_TRANSFORMER

#include "action.hpp"

class WingedMesh;
class ActionOnWMesh;

class ActionTransformer : public Action {
  public:
          ActionTransformer             (WingedMesh&, ActionOnWMesh*);
          ActionTransformer             (const ActionTransformer&) = delete;
          ActionTransformer             (      ActionTransformer&&);
    const ActionTransformer & operator= (const ActionTransformer&) = delete;
         ~ActionTransformer             ();

    void undo ();
    void redo ();

  private:
    class Impl;
    Impl* impl;
};

#endif

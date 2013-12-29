#ifndef DILAY_ACTION_CARVE
#define DILAY_ACTION_CARVE

#include <list>
#include "action.hpp"
#include "macro.hpp"

class Id;
class WingedMesh;

class ActionCarve : public Action {
  public: 
    DECLARE_ACTION_BIG5 (ActionCarve)

    void run  (WingedMesh&, const glm::vec3&, float);
    void undo ();
    void redo ();

  private:
    class Impl;
    Impl* impl;
};

#endif

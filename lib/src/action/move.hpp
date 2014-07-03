#ifndef DILAY_ACTION_MOVE
#define DILAY_ACTION_MOVE

#include <glm/fwd.hpp>
#include "action.hpp"
#include "macro.hpp"

class SphereMesh;

class ActionMove : public Action {
  public: 
    DECLARE_BIG3 (ActionMove)

    void translate (SphereMesh&, const glm::vec3&);
    void position  (SphereMesh&, const glm::vec3&);

  private:
    void runUndo ();
    void runRedo ();

    class Impl;
    Impl* impl;
};

#endif

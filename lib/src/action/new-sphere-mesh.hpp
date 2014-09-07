#ifndef DILAY_ACTION_NEW_SPHERE_MESH
#define DILAY_ACTION_NEW_SPHERE_MESH

#include <glm/fwd.hpp>
#include "action.hpp"
#include "macro.hpp"

class SphereMesh;

class ActionNewSphereMesh : public Action {
  public: 
    DECLARE_BIG3 (ActionNewSphereMesh)

    SphereMesh& run (const glm::vec3&, float);

  private:
    void runUndo ();
    void runRedo ();

    IMPLEMENTATION
};

#endif

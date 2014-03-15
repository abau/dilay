#ifndef DILAY_ACTION_NEW_SPHERE_MESH
#define DILAY_ACTION_NEW_SPHERE_MESH

#include "action.hpp"
#include "macro.hpp"

class SphereMesh;

class ActionNewSphereMesh : public Action {
  public: 
    DECLARE_BIG3 (ActionNewSphereMesh)

    SphereMesh& run ();

  private:
    void runUndo ();
    void runRedo ();

    class Impl;
    Impl* impl;
};

#endif

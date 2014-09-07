#ifndef DILAY_ACTION_DELETE_SPHERE_MESH
#define DILAY_ACTION_DELETE_SPHERE_MESH

#include "action.hpp"
#include "macro.hpp"

class SphereMesh;
class SphereMeshNode;

class ActionDeleteSMesh : public Action {
  public: 
    DECLARE_BIG3 (ActionDeleteSMesh)

    void deleteNodesRecursive (SphereMesh&, SphereMeshNode&);
    void deleteMesh           (SphereMesh&);

  private:
    void runUndo ();
    void runRedo ();

    IMPLEMENTATION
};

#endif

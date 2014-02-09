#ifndef DILAY_ACTION_NEW_MESH
#define DILAY_ACTION_NEW_MESH

#include "action.hpp"
#include "macro.hpp"

class WingedMesh;
enum class MeshType;

class ActionNewMesh : public Action {
  public: 
    DECLARE_ACTION_BIG6 (ActionNewMesh)

    WingedMesh& icosphere (MeshType, unsigned int);

  private:
    void runUndo ();
    void runRedo ();

    class Impl;
    Impl* impl;
};

#endif

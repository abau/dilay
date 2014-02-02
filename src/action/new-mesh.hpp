#ifndef DILAY_ACTION_NEW_MESH
#define DILAY_ACTION_NEW_MESH

#include "action.hpp"
#include "macro.hpp"

class WingedMesh;

class ActionNewMesh : public Action {
  public: 
    DECLARE_ACTION_BIG6 (ActionNewMesh)

    WingedMesh& cube      ();
    WingedMesh& sphere    (unsigned int, unsigned int);
    WingedMesh& icosphere (unsigned int);

  private:
    void runUndo ();
    void runRedo ();

    class Impl;
    Impl* impl;
};

#endif

#ifndef DILAY_ACTION_NEW_WINGED_MESH
#define DILAY_ACTION_NEW_WINGED_MESH

#include "action.hpp"
#include "macro.hpp"

class WingedMesh;
class Mesh;
enum class MeshType;

class ActionNewWingedMesh : public Action {
  public: 
    DECLARE_BIG3 (ActionNewWingedMesh)

    WingedMesh& run (MeshType, const Mesh&);

  private:
    void runUndo ();
    void runRedo ();

    class Impl;
    Impl* impl;
};

#endif

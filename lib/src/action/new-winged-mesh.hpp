#ifndef DILAY_ACTION_NEW_WINGED_MESH
#define DILAY_ACTION_NEW_WINGED_MESH

#include "action.hpp"
#include "macro.hpp"

class WingedMesh;
enum class MeshType;
class MeshDefinition;

class ActionNewWingedMesh : public Action {
  public: 
    DECLARE_BIG3 (ActionNewWingedMesh)

    WingedMesh& run (MeshType, const MeshDefinition&);

  private:
    void runUndo ();
    void runRedo ();

    IMPLEMENTATION
};

#endif

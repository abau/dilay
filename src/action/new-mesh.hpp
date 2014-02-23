#ifndef DILAY_ACTION_NEW_MESH
#define DILAY_ACTION_NEW_MESH

#include "action.hpp"
#include "macro.hpp"

class WingedMesh;
class Mesh;
enum class MeshType;

class ActionNewMesh : public Action {
  public: 
    DECLARE_BIG3 (ActionNewMesh)

    WingedMesh& newMesh (MeshType, const Mesh&);

  private:
    void runUndo ();
    void runRedo ();

    class Impl;
    Impl* impl;
};

#endif

#ifndef DILAY_ACTION_DELETE_WINGED_MESH
#define DILAY_ACTION_DELETE_WINGED_MESH

#include "action.hpp"
#include "macro.hpp"

class WingedMesh;
enum class MeshType;

class ActionDeleteWMesh : public Action {
  public: 
    DECLARE_BIG3 (ActionDeleteWMesh)

    void deleteMesh (MeshType, WingedMesh&);

  private:
    void runUndo () const;
    void runRedo () const;

    IMPLEMENTATION
};

#endif

#ifndef DILAY_ACTION_FROM_MESH
#define DILAY_ACTION_FROM_MESH

#include "action/on.hpp"
#include "macro.hpp"

class WingedMesh;
class Mesh;

class ActionFromMesh : public ActionOn <WingedMesh> {
  public: 
    DECLARE_BIG3 (ActionFromMesh)

    void run (WingedMesh&, const Mesh&);

  private:
    void runUndo (WingedMesh&);
    void runRedo (WingedMesh&);

    IMPLEMENTATION
};

#endif

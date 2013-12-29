#ifndef DILAY_ACTION_FROM_MESH
#define DILAY_ACTION_FROM_MESH

#include "action.hpp"
#include "macro.hpp"

class WingedMesh;
class Mesh;

class ActionFromMesh : public Action {
  public: 
    DECLARE_ACTION_BIG5 (ActionFromMesh)

    void cube (WingedMesh&, float);
    void undo ();
    void redo ();

  private:
    class Impl;
    Impl* impl;
};

#endif

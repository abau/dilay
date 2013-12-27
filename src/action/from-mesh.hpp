#ifndef DILAY_ACTION_FROM_MESH
#define DILAY_ACTION_FROM_MESH

#include "action.hpp"

class WingedMesh;
class Mesh;

class ActionFromMesh : public Action {
  public: ActionFromMesh            ();
          ActionFromMesh            (const ActionFromMesh&) = delete;
    const ActionFromMesh& operator= (const ActionFromMesh&) = delete;
         ~ActionFromMesh            ();

    void cube (WingedMesh&, float);
    void undo ();
    void redo ();

  private:
    class Impl;
    Impl* impl;
};

#endif

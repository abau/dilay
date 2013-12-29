#ifndef DILAY_ACTION_ON_WINGED_MESH
#define DILAY_ACTION_ON_WINGED_MESH

class WingedMesh;

class ActionOnWMesh {
  public:
    virtual void undo (WingedMesh&) = 0;
    virtual void redo (WingedMesh&) = 0;
};

#endif

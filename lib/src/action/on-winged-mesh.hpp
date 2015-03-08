#ifndef DILAY_ACTION_ON_WINGED_MESH
#define DILAY_ACTION_ON_WINGED_MESH

class WingedMesh;

class ActionOnWMesh {
  public:
    virtual ~ActionOnWMesh () {}

    void undo (WingedMesh& m) const { this->runUndo (m); }
    void redo (WingedMesh& m) const { this->runRedo (m); }

  private:
    virtual void runUndo (WingedMesh&) const = 0;
    virtual void runRedo (WingedMesh&) const = 0;
};

#endif

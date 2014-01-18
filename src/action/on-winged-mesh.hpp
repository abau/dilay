#ifndef DILAY_ACTION_ON_WINGED_MESH
#define DILAY_ACTION_ON_WINGED_MESH

class WingedMesh;

class ActionOnWMesh {
  public:
    virtual ~ActionOnWMesh () {}

    void undo (WingedMesh& m) { this->runUndo (m); }
    void redo (WingedMesh& m) { this->runRedo (m); }

  private:
    virtual void runUndo (WingedMesh&) = 0;
    virtual void runRedo (WingedMesh&) = 0;
};

#endif

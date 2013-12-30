#ifndef DILAY_ACTION_FROM_MESH
#define DILAY_ACTION_FROM_MESH

#include "action/on-winged-mesh.hpp"
#include "macro.hpp"

class WingedMesh;

class ActionFromMesh : public ActionOnWMesh {
  public: 
    DECLARE_ACTION_BIG6 (ActionFromMesh)

    void cube      (WingedMesh&);
    void sphere    (WingedMesh&, unsigned int, unsigned int);
    void icosphere (WingedMesh&, unsigned int);
    void undo      (WingedMesh&);
    void redo      (WingedMesh&);

  private:
    class Impl;
    Impl* impl;
};

#endif

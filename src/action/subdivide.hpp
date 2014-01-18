#ifndef DILAY_ACTION_SUBDIVIDE
#define DILAY_ACTION_SUBDIVIDE

#include <list>
#include "action/on-winged-mesh.hpp"
#include "macro.hpp"

class WingedFace;
class WingedMesh;
class Id;

class ActionSubdivide : public ActionOnWMesh {
  public: 
    DECLARE_ACTION_BIG6 (ActionSubdivide)

    void run  (WingedMesh&, WingedFace&, std::list <Id>* = nullptr);

  private:
    void runUndo (WingedMesh&);
    void runRedo (WingedMesh&);

    class Impl;
    Impl* impl;
};

#endif

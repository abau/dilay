#ifndef DILAY_ACTION_SUBDIVIDE
#define DILAY_ACTION_SUBDIVIDE

#include <list>
#include "action.hpp"
#include "macro.hpp"

class WingedFace;
class WingedMesh;
class Id;

class ActionSubdivide : public Action {
  public: 
    DECLARE_ACTION_BIG5 (ActionSubdivide)

    WingedFace& run  (WingedMesh&, WingedFace&, std::list <Id>* = nullptr);
    void undo ();
    void redo ();

  private:
    class Impl;
    Impl* impl;
};

#endif

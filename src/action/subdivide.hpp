#ifndef DILAY_ACTION_SUBDIVIDE
#define DILAY_ACTION_SUBDIVIDE

#include <list>
#include "action.hpp"

class WingedFace;
class WingedMesh;
class Id;

class ActionSubdivide : public Action {
  public: 
          ActionSubdivide            ();
          ActionSubdivide            (const ActionSubdivide&) = delete;
    const ActionSubdivide& operator= (const ActionSubdivide&) = delete;
         ~ActionSubdivide            ();

    WingedFace& run  (WingedMesh&, WingedFace&, std::list <Id>* = nullptr);
    void undo ();
    void redo ();

  private:
    class Impl;
    Impl* impl;
};

#endif

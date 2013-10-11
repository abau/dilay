#ifndef DILAY_ACTION_SUBDIVIDE
#define DILAY_ACTION_SUBDIVIDE

#include "action.hpp"

class WingedFace;
class WingedMesh;

class ActionSubdivide : public Action {
  public: 
          ActionSubdivide            ();
          ActionSubdivide            (const ActionSubdivide&) = delete;
    const ActionSubdivide& operator= (const ActionSubdivide&) = delete;
         ~ActionSubdivide            ();

    void run  (WingedMesh&, WingedFace&);
    void undo ();
    void redo ();

  private:
    class Impl;
    Impl* impl;
};

#endif

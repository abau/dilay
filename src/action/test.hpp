#ifndef DILAY_ACTION_TEST
#define DILAY_ACTION_TEST

#include "action.hpp"

class WingedMesh;
class WingedEdge;

class ActionTest : public Action {
  public: ActionTest            ();
          ActionTest            (const ActionTest&) = delete;
    const ActionTest& operator= (const ActionTest&) = delete;
         ~ActionTest            ();

    void run  (WingedMesh&, WingedEdge&);
    void undo ();
    void redo ();
  private:
    class Impl;
    Impl* impl;
};
#endif

#ifndef DILAY_PARTIAL_ACTION_DELETE_T_EDGES
#define DILAY_PARTIAL_ACTION_DELETE_T_EDGES

#include "action.hpp"
#include "fwd-glm.hpp"

class WingedFace;
class WingedMesh;

class PADeleteTEdges : public Action {
  public: PADeleteTEdges            ();
          PADeleteTEdges            (const PADeleteTEdges&) = delete;
    const PADeleteTEdges& operator= (const PADeleteTEdges&) = delete;
         ~PADeleteTEdges            ();

    void run (WingedMesh&, std::unordered_set <WingedFace*>&);

    void undo ();
    void redo ();
  private:
    class Impl;
    Impl* impl;
};
#endif

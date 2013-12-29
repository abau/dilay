#ifndef DILAY_PARTIAL_ACTION_DELETE_T_EDGES
#define DILAY_PARTIAL_ACTION_DELETE_T_EDGES

#include "action.hpp"
#include "fwd-glm.hpp"
#include "macro.hpp"

class WingedFace;
class WingedMesh;

class PADeleteTEdges : public Action {
  public: 
    DECLARE_ACTION_BIG5 (PADeleteTEdges)

    void run (WingedMesh&, std::unordered_set <WingedFace*>&);

    void undo ();
    void redo ();
  private:
    class Impl;
    Impl* impl;
};
#endif

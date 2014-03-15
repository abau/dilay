#ifndef DILAY_PARTIAL_ACTION_DELETE_T_EDGES
#define DILAY_PARTIAL_ACTION_DELETE_T_EDGES

#include <glm/fwd.hpp>
#include "action/on.hpp"
#include "macro.hpp"

class WingedFace;
class WingedMesh;

class PADeleteTEdges : public ActionOn <WingedMesh> {
  public: 
    DECLARE_BIG3 (PADeleteTEdges)

    void run (WingedMesh&, std::unordered_set <WingedFace*>&);

  private:
    void runUndo (WingedMesh&);
    void runRedo (WingedMesh&);

    class Impl;
    Impl* impl;
};
#endif

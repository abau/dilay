#ifndef DILAY_PARTIAL_ACTION_DELETE_T_EDGES
#define DILAY_PARTIAL_ACTION_DELETE_T_EDGES

#include "action/on-winged-mesh.hpp"
#include "fwd-glm.hpp"
#include "macro.hpp"

class WingedFace;
class WingedMesh;

class PADeleteTEdges : public ActionOnWMesh {
  public: 
    DECLARE_ACTION_BIG6 (PADeleteTEdges)

    void run (WingedMesh&, std::unordered_set <WingedFace*>&);

    void undo (WingedMesh&);
    void redo (WingedMesh&);
  private:
    class Impl;
    Impl* impl;
};
#endif

#ifndef DILAY_ACTION_SUBDIVIDE
#define DILAY_ACTION_SUBDIVIDE

#include <unordered_set>
#include "action/on.hpp"
#include "macro.hpp"

class WingedMesh;
class WingedFace;

class ActionSubdivide : public ActionOn <WingedMesh> {
  public: 
    DECLARE_BIG3 (ActionSubdivide)

    void run (WingedMesh&, const std::unordered_set <WingedFace*>&
                         ,       std::unordered_set <WingedFace*>&);

  private:
    void runUndo (WingedMesh&);
    void runRedo (WingedMesh&);

    IMPLEMENTATION
};

#endif

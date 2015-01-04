#ifndef DILAY_ACTION_MODIFY_WINGED_MESH
#define DILAY_ACTION_MODIFY_WINGED_MESH

#include <glm/fwd.hpp>
#include "action/on.hpp"
#include "macro.hpp"

class WingedMesh;

class ActionModifyWMesh : public ActionOn <WingedMesh> {
  public: 
    DECLARE_BIG3 (ActionModifyWMesh)

    void translate (WingedMesh&, const glm::vec3&);
    void position  (WingedMesh&, const glm::vec3&);
    void scale     (WingedMesh&, const glm::vec3&);

  private:
    void runUndo (WingedMesh&) const;
    void runRedo (WingedMesh&) const;

    IMPLEMENTATION
};

#endif

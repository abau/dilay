#ifndef DILAY_ACTION_MODIFY_WINGED_MESH
#define DILAY_ACTION_MODIFY_WINGED_MESH

#include <glm/fwd.hpp>
#include "action/on.hpp"
#include "macro.hpp"

class WingedMesh;

class ActionModifyWMesh : public ActionOn <WingedMesh> {
  public: 
    DECLARE_BIG3 (ActionModifyWMesh)

    void position       (WingedMesh&, const glm::vec3&);
    void rotationMatrix (WingedMesh&, const glm::mat4x4&);
    void scaling        (WingedMesh&, const glm::vec3&);

  private:
    void runUndo (WingedMesh&);
    void runRedo (WingedMesh&);

    class Impl;
    Impl* impl;
};

#endif

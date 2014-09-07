#ifndef DILAY_ACTION_MODIFY_SPHERE_MESH
#define DILAY_ACTION_MODIFY_SPHERE_MESH

#include <glm/fwd.hpp>
#include "action/on.hpp"
#include "macro.hpp"

class SphereMesh;
class SphereMeshNode;

class ActionModifySMesh : public ActionOn <SphereMesh> {
  public: 
    DECLARE_BIG3 (ActionModifySMesh)

    void newNode    (SphereMesh&, SphereMeshNode*, const glm::vec3&, float);
    // Node to delete must be leaf
    void deleteNode (SphereMesh&, SphereMeshNode&);

  private:
    void runUndo (SphereMesh&);
    void runRedo (SphereMesh&);

    IMPLEMENTATION
};

#endif

#include "action/new-sphere-mesh.hpp"
#include "action/ids.hpp"
#include "action/modify-sphere-mesh.hpp"
#include "state.hpp"
#include "sphere/mesh.hpp"
#include "scene.hpp"
#include "id.hpp"
#include "mesh-type.hpp"

struct ActionNewSphereMesh :: Impl {
  ActionIds         ids;
  ActionModifySMesh addRootAction;

  SphereMesh& run (const glm::vec3& pos, float r) {
    SphereMesh& sMesh = State::scene ().newSphereMesh ();
    this->ids.setMesh           (0, sMesh);
    this->addRootAction.newNode (sMesh, nullptr, pos, r);
    return sMesh;
  }

  void runUndo () {
    SphereMesh& sMesh = this->ids.getSphereMesh (0);
    this->addRootAction.undo   (sMesh);
    State::scene ().deleteMesh (MeshType::Sphere, sMesh.id ());
  }

  void runRedo () {
    SphereMesh& sMesh = State::scene ().newSphereMesh (this->ids.getIdRef (0));
    this->addRootAction.redo (sMesh);
  }
};

DELEGATE_BIG3 (ActionNewSphereMesh)

DELEGATE2 (SphereMesh&, ActionNewSphereMesh, run, const glm::vec3&, float)
DELEGATE  (void       , ActionNewSphereMesh, runUndo)
DELEGATE  (void       , ActionNewSphereMesh, runRedo)

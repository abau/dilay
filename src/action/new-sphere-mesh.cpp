#include <glm/glm.hpp>
#include "action/new-sphere-mesh.hpp"
#include "state.hpp"
#include "action/ids.hpp"
#include "sphere/mesh.hpp"
#include "scene.hpp"

struct ActionNewSphereMesh :: Impl {
  ActionIds ids;
  glm::vec3 position;

  SphereMesh& run (const glm::vec3& p) {
    SphereMesh& sMesh = State::scene ().newSphereMesh ();
    sMesh.root ().position (p);

    this->ids.setMesh (0, sMesh);
    this->position = p;
    return sMesh;
  }

  void runUndo () {
    State::scene ().removeWingedMesh (this->ids.getIdRef (0));
  }

  void runRedo () {
    SphereMesh& sMesh = State::scene ().newSphereMesh (this->ids.getIdRef (0));
    sMesh.root ().position (this->position);
  }
};

DELEGATE_BIG3 (ActionNewSphereMesh)

DELEGATE1 (SphereMesh&, ActionNewSphereMesh, run, const glm::vec3&)
DELEGATE  (void       , ActionNewSphereMesh, runUndo)
DELEGATE  (void       , ActionNewSphereMesh, runRedo)

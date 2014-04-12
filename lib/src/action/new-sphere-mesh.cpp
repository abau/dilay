#include <glm/glm.hpp>
#include "action/new-sphere-mesh.hpp"
#include "state.hpp"
#include "action/ids.hpp"
#include "sphere/mesh.hpp"
#include "scene.hpp"

struct ActionNewSphereMesh :: Impl {
  ActionIds ids;
  glm::vec3 position;
  float     radius;

  SphereMesh& run (const glm::vec3& p, float r) {
    SphereMesh& sMesh = State::scene ().newSphereMesh ();
    sMesh.root ().position (p);
    sMesh.root ().radius   (r);

    this->ids.setMesh (0, sMesh);
    this->position = p;
    this->radius   = r;
    return sMesh;
  }

  void runUndo () {
    State::scene ().removeWingedMesh (this->ids.getIdRef (0));
  }

  void runRedo () {
    SphereMesh& sMesh = State::scene ().newSphereMesh (this->ids.getIdRef (0));
    sMesh.root ().position (this->position);
    sMesh.root ().radius   (this->radius);
  }
};

DELEGATE_BIG3 (ActionNewSphereMesh)

DELEGATE2 (SphereMesh&, ActionNewSphereMesh, run, const glm::vec3&, float)
DELEGATE  (void       , ActionNewSphereMesh, runUndo)
DELEGATE  (void       , ActionNewSphereMesh, runRedo)

#include "action/new-sphere-mesh.hpp"
#include "state.hpp"
#include "action/ids.hpp"
#include "sphere/mesh.hpp"
#include "scene.hpp"

struct ActionNewSphereMesh :: Impl {
  ActionIds ids;

  SphereMesh& run () {
    SphereMesh& sMesh = State::scene ().newSphereMesh ();

    this->ids.setMesh (0, &sMesh);
    return sMesh;
  }

  void runUndo () {
    State::scene ().removeWingedMesh (this->ids.getIdRef (0));
  }

  void runRedo () {
    State::scene ().newSphereMesh (this->ids.getIdRef (0));
  }
};

DELEGATE_BIG3 (ActionNewSphereMesh)

DELEGATE (SphereMesh&, ActionNewSphereMesh, run)
DELEGATE (void       , ActionNewSphereMesh, runUndo)
DELEGATE (void       , ActionNewSphereMesh, runRedo)

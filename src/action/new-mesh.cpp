#include "action/new-mesh.hpp"
#include "state.hpp"
#include "action/from-mesh.hpp"
#include "action/ids.hpp"
#include "action/unit/on-winged-mesh.hpp"
#include "winged/mesh.hpp"
#include "scene.hpp"

struct ActionNewMesh :: Impl {
  ActionUnitOnWMesh actions;
  ActionIds         ids;

  WingedMesh& icosphere (unsigned int numSubdivisions) {
    WingedMesh& mesh = State::scene ().newWingedMesh ();

    this->ids.setMesh (0, &mesh);
    this->actions.add <ActionFromMesh> ()->icosphere (mesh, numSubdivisions);
    return mesh;
  }

  void runUndo () {
    this->actions.undo (this->ids.getMesh (0));
    State::scene ().removeWingedMesh (this->ids.getIdRef (0));
  }

  void runRedo () {
    WingedMesh& mesh = State::scene ().newWingedMesh (this->ids.getIdRef (0));
    this->actions.redo (mesh);
  }
};


DELEGATE_ACTION_BIG6 (ActionNewMesh)

DELEGATE1 (WingedMesh&, ActionNewMesh, icosphere, unsigned int)
DELEGATE  (void       , ActionNewMesh, runUndo)
DELEGATE  (void       , ActionNewMesh, runRedo)

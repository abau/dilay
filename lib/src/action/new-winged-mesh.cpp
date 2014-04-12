#include "action/new-winged-mesh.hpp"
#include "state.hpp"
#include "action/from-mesh.hpp"
#include "action/ids.hpp"
#include "action/unit/on.hpp"
#include "winged/mesh.hpp"
#include "scene.hpp"
#include "mesh-type.hpp"

struct ActionNewWingedMesh :: Impl {
  ActionUnitOn <WingedMesh> actions;
  ActionIds                 ids;
  MeshType                  meshType;

  WingedMesh& run (MeshType t, const Mesh& mesh) {
    WingedMesh& wMesh = State::scene ().newWingedMesh (t);

    this->meshType = t;
    this->ids.setMesh (0, wMesh);
    this->actions.add <ActionFromMesh> ()->fromMesh (wMesh, mesh);
    return wMesh;
  }

  void runUndo () {
    this->actions.undo (this->ids.getWingedMesh (0));
    State::scene ().removeWingedMesh (this->ids.getIdRef (0));
  }

  void runRedo () {
    WingedMesh& mesh = State::scene ().newWingedMesh (this->meshType, this->ids.getIdRef (0));
    this->actions.redo (mesh);
  }
};

DELEGATE_BIG3 (ActionNewWingedMesh)

DELEGATE2 (WingedMesh&, ActionNewWingedMesh, run, MeshType, const Mesh&)
DELEGATE  (void       , ActionNewWingedMesh, runUndo)
DELEGATE  (void       , ActionNewWingedMesh, runRedo)

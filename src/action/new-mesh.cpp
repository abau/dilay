#include "action/new-mesh.hpp"
#include "state.hpp"
#include "action/from-mesh.hpp"
#include "action/ids.hpp"
#include "action/unit/on-winged-mesh.hpp"
#include "winged/mesh.hpp"
#include "scene.hpp"
#include "mesh-type.hpp"

struct ActionNewMesh :: Impl {
  ActionUnitOnWMesh actions;
  ActionIds         ids;
  MeshType          meshType;

  WingedMesh& newMesh (MeshType t, const Mesh& mesh) {
    WingedMesh& wMesh = State::scene ().newWingedMesh (t);

    this->meshType = t;
    this->ids.setMesh (0, &wMesh);
    this->actions.add <ActionFromMesh> ()->fromMesh (wMesh, mesh);
    return wMesh;
  }

  void runUndo () {
    this->actions.undo (this->ids.getMesh (0));
    State::scene ().removeWingedMesh (this->ids.getIdRef (0));
  }

  void runRedo () {
    WingedMesh& mesh = State::scene ().newWingedMesh (this->meshType, this->ids.getIdRef (0));
    this->actions.redo (mesh);
  }
};


DELEGATE_BIG3 (ActionNewMesh)

DELEGATE2 (WingedMesh&, ActionNewMesh, newMesh, MeshType, const Mesh&)
DELEGATE  (void       , ActionNewMesh, runUndo)
DELEGATE  (void       , ActionNewMesh, runRedo)

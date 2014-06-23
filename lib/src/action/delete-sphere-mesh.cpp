#include "action/delete-sphere-mesh.hpp"
#include "action/modify-sphere-mesh.hpp"
#include "action/unit/on.hpp"
#include "action/ids.hpp"
#include "sphere/mesh.hpp"
#include "state.hpp"
#include "scene.hpp"
#include "mesh-type.hpp"
#include "id.hpp"

struct ActionDeleteSMesh::Impl {
  ActionUnitOn <SphereMesh> actions;
  ActionIds                 ids;
  
  void deleteNodesRecursive (SphereMesh& mesh, SphereMeshNode& node) {
    this->ids.setMesh             (0, mesh);
    this->runDeleteNodesRecursive (mesh, node);
  }

  void runDeleteNodesRecursive (SphereMesh& mesh, SphereMeshNode& node) {
    if (node.numChildren () == 0) {
      this->actions.add <ActionModifySMesh> ().deleteNode (mesh, node);
    }
    else {
      this->runDeleteNodesRecursive (mesh, node.firstChild ());
      this->runDeleteNodesRecursive (mesh, node);
    }
  }

  void deleteMesh (SphereMesh& mesh) {
    this->ids.setMesh (0, mesh);
    if (mesh.hasRoot ()) {
      this->deleteNodesRecursive (mesh, mesh.root ());
    }
    State::scene ().deleteMesh (MeshType::Sphere, mesh.id ());
  }

  void runUndo () {
    SphereMesh& mesh = State::scene ().newSphereMesh (this->ids.getIdRef (0));
    this->actions.undo (mesh);
  }
    
  void runRedo () {
    SphereMesh& mesh = this->ids.getSphereMesh (0);
    this->actions.redo (mesh);
  }
};

DELEGATE_BIG3 (ActionDeleteSMesh)
DELEGATE2 (void, ActionDeleteSMesh, deleteNodesRecursive, SphereMesh&, SphereMeshNode&)
DELEGATE1 (void, ActionDeleteSMesh, deleteMesh, SphereMesh&)
DELEGATE  (void, ActionDeleteSMesh, runUndo)
DELEGATE  (void, ActionDeleteSMesh, runRedo)

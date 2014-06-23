#include "action/delete-winged-mesh.hpp"
#include "action/ids.hpp"
#include "action/from-mesh.hpp"
#include "winged/mesh.hpp"
#include "state.hpp"
#include "scene.hpp"
#include "mesh-type.hpp"
#include "mesh.hpp"
#include "id.hpp"

struct ActionDeleteWMesh::Impl {
  ActionIds ids;
  Mesh      mesh;
  MeshType  meshType;
  
  void deleteMesh (MeshType t, WingedMesh& wMesh) {
    this->ids.setMesh (0, wMesh);
    this->meshType = t;
    this->mesh     = wMesh.mesh ();
    State::scene ().deleteMesh (this->meshType, wMesh.id ());
  }

  void runUndo () {
    WingedMesh& wMesh = State::scene ().newWingedMesh (this->meshType, this->ids.getIdRef (0));
    ActionFromMesh fromMesh;
    fromMesh.run (wMesh, this->mesh);
  }
    
  void runRedo () {
    State::scene ().deleteMesh (this->meshType, this->ids.getIdRef (0));
  }
};

DELEGATE_BIG3 (ActionDeleteWMesh)
DELEGATE2 (void, ActionDeleteWMesh, deleteMesh, MeshType, WingedMesh&)
DELEGATE  (void, ActionDeleteWMesh, runUndo)
DELEGATE  (void, ActionDeleteWMesh, runRedo)

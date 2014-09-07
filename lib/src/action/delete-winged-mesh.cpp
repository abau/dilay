#include <memory>
#include "action/delete-winged-mesh.hpp"
#include "action/ids.hpp"
#include "id.hpp"
#include "mesh-type.hpp"
#include "mesh.hpp"
#include "scene.hpp"
#include "state.hpp"
#include "winged/mesh.hpp"

struct ActionDeleteWMesh::Impl {
  ActionIds                    ids;
  MeshType                     meshType;
  std::unique_ptr <WingedMesh> mesh;
  
  void deleteMesh (MeshType t, WingedMesh& wMesh) {
    this->ids.setMesh (0, wMesh);
    this->meshType = t;
    this->mesh.reset (new WingedMesh (std::move (wMesh)));
    State::scene ().deleteMesh (this->meshType, this->ids.getIdRef (0));
  }

  void runUndo () {
    assert (this->mesh);
    State::scene ().newWingedMesh (this->meshType, std::move (*this->mesh));
  }
    
  void runRedo () {
    this->mesh.reset (new WingedMesh (std::move (this->ids.getWingedMesh (0))));
    State::scene ().deleteMesh (this->meshType, this->ids.getIdRef (0));
  }
};

DELEGATE_BIG3 (ActionDeleteWMesh)
DELEGATE2 (void, ActionDeleteWMesh, deleteMesh, MeshType, WingedMesh&)
DELEGATE  (void, ActionDeleteWMesh, runUndo)
DELEGATE  (void, ActionDeleteWMesh, runRedo)

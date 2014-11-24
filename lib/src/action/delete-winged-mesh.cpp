#include "action/data.hpp"
#include "action/delete-winged-mesh.hpp"
#include "action/unit/on.hpp"
#include "mesh-type.hpp"
#include "octree.hpp"
#include "partial-action/modify-winged-edge.hpp"
#include "partial-action/modify-winged-face.hpp"
#include "partial-action/modify-winged-mesh.hpp"
#include "partial-action/modify-winged-vertex.hpp"
#include "scene.hpp"
#include "state.hpp"
#include "winged/edge.hpp"
#include "winged/face.hpp"
#include "winged/mesh.hpp"
#include "winged/vertex.hpp"

struct ActionDeleteWMesh::Impl {
  ActionUnitOn <WingedMesh> actions;
  ActionData   <MeshType>   data;
  
  void deleteMesh (MeshType t, WingedMesh& mesh) {
    this->data.index (mesh);
    this->data.value (t);

    // reset entities
    mesh.octree ().forEachFace ([this] (WingedFace& f) {
      this->actions.add <PAModifyWFace> ().reset (f);
    });
    mesh.forEachVertex ([this] (WingedVertex& v) {
      this->actions.add <PAModifyWVertex> ().reset (v);
    });
    mesh.forEachEdge ([this] (WingedEdge& e) {
      this->actions.add <PAModifyWEdge> ().reset (e);
    });

    // delete entities
    mesh.octree ().forEachFace ([this,&mesh] (WingedFace& f) {
      this->actions.add <PAModifyWMesh> ().deleteFace (mesh, f);
    });
    mesh.forEachVertex ([this,&mesh] (WingedVertex& v) {
      this->actions.add <PAModifyWMesh> ().deleteVertex (mesh, v);
    });
    mesh.forEachEdge ([this,&mesh] (WingedEdge& e) {
      this->actions.add <PAModifyWMesh> ().deleteEdge (mesh, e);
    });
    State::scene ().deleteMesh (mesh);
  }

  void runUndo () const {
    WingedMesh& mesh = State::scene ().newWingedMesh ( this->data.index ()
                                                     , this->data.value <MeshType> () );
    this->actions.undo (mesh);

    mesh.writeAllIndices ();
    mesh.writeAllNormals ();
    mesh.bufferData      ();
  }
    
  void runRedo () const {
    WingedMesh& mesh = State::scene ().wingedMeshRef (this->data.index ());
    this->actions.redo (mesh);
    State::scene ().deleteMesh (mesh);
  }
};

DELEGATE_BIG3   (ActionDeleteWMesh)
DELEGATE2       (void, ActionDeleteWMesh, deleteMesh, MeshType, WingedMesh&)
DELEGATE_CONST  (void, ActionDeleteWMesh, runUndo)
DELEGATE_CONST  (void, ActionDeleteWMesh, runRedo)

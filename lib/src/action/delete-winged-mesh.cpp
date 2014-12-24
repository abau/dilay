#include "action/delete-winged-mesh.hpp"
#include "action/unit/on.hpp"
#include "maybe.hpp"
#include "octree.hpp"
#include "partial-action/modify-winged-edge.hpp"
#include "partial-action/modify-winged-face.hpp"
#include "partial-action/modify-winged-mesh.hpp"
#include "partial-action/modify-winged-vertex.hpp"
#include "scene.hpp"
#include "winged/edge.hpp"
#include "winged/face.hpp"
#include "winged/mesh.hpp"
#include "winged/vertex.hpp"

struct ActionDeleteWMesh::Impl {
  Scene*                      scene;
  ActionUnitOn <WingedMesh>   actions;
  Maybe        <unsigned int> index;
  
  void run (Scene& s, WingedMesh& mesh) {
    this->scene = &s;
    this->index.set (mesh.index ());

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
    this->scene->deleteMesh (mesh);
  }

  void runUndo () const {
    WingedMesh& mesh = this->scene->newWingedMesh (this->index.getRef ());
    this->actions.undo (mesh);

    mesh.writeAllIndices ();
    mesh.writeAllNormals ();
    mesh.bufferData      ();
  }
    
  void runRedo () const {
    WingedMesh& mesh = this->scene->wingedMeshRef (this->index.getRef ());
    this->actions.redo (mesh);
    this->scene->deleteMesh (mesh);
  }
};

DELEGATE_BIG3   (ActionDeleteWMesh)
DELEGATE2       (void, ActionDeleteWMesh, run, Scene&, WingedMesh&)
DELEGATE_CONST  (void, ActionDeleteWMesh, runUndo)
DELEGATE_CONST  (void, ActionDeleteWMesh, runRedo)

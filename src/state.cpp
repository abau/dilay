#include "state.hpp"
#include "mesh.hpp"
#include "winged-mesh.hpp"
#include "winged-mesh-util.hpp"
#include "camera.hpp"
#include "cursor.hpp"
#include "macro.hpp"

struct StateImpl {
  WingedMesh mesh;
  Camera     camera;
  Cursor     cursor;

  void initialize () { 
    WingedMeshUtil :: fromMesh (this->mesh, Mesh :: sphere (1.0f,10,20));
    this->mesh.bufferData ();
    this->camera.initialize ();
    this->cursor.initialize ();
  }

  void render () {
    this->mesh.render ();
    this->cursor.render ();
  }

  void setMesh (const Mesh& mesh) {
    WingedMeshUtil :: fromMesh (this->mesh,mesh);
  }
};

GLOBAL               (State)
DELEGATE_CONSTRUCTOR (State)
DELEGATE_DESTRUCTOR  (State)

ACCESS (WingedMesh&, State, mesh)
ACCESS (Camera&    , State, camera)
ACCESS (Cursor&    , State, cursor)

DELEGATE  (void, State, initialize)
DELEGATE  (void, State, render)
DELEGATE1 (void, State, setMesh, const Mesh&)

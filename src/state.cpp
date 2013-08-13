#include <list>
#include "state.hpp"
#include "mesh.hpp"
#include "winged-mesh.hpp"
#include "winged-util.hpp"
#include "camera.hpp"
#include "cursor.hpp"
#include "macro.hpp"
#include "history.hpp"
#include "view-mouse-movement.hpp"

struct State::Impl {
  WingedMesh    mesh;
  Camera        camera;
  Cursor        cursor;
  History       history;
  MouseMovement mouseMovement;

  void initialize () { 
    WingedUtil :: fromMesh (this->mesh, Mesh :: sphere (1.0f,100,200));
    //WingedUtil :: fromMesh (this->mesh, Mesh :: sphere (1.0f,10,20));
    //WingedUtil :: fromMesh (this->mesh, Mesh :: cube (1.0f));
    this->camera.initialize ();
    this->cursor.initialize ();
  }

  void render () {
    this->mesh.render ();
    this->cursor.render ();
  }

  void setMesh (const Mesh& mesh) {
    WingedUtil :: fromMesh (this->mesh,mesh);
  }
};

GLOBAL               (State)
DELEGATE_CONSTRUCTOR (State)
DELEGATE_DESTRUCTOR  (State)

ACCESS_GLOBAL (WingedMesh&   , State, mesh)
ACCESS_GLOBAL (Camera&       , State, camera)
ACCESS_GLOBAL (Cursor&       , State, cursor)
ACCESS_GLOBAL (MouseMovement&, State, mouseMovement)

DELEGATE_GLOBAL  (void, State, initialize)
DELEGATE_GLOBAL  (void, State, render)
DELEGATE1_GLOBAL (void, State, setMesh, const Mesh&)

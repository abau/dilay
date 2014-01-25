#include <list>
#include "state.hpp"
#include "mesh.hpp"
#include "winged-mesh.hpp"
#include "camera.hpp"
#include "cursor.hpp"
#include "macro.hpp"
#include "history.hpp"
#include "view/mouse-movement.hpp"
#include "id.hpp"
#include "action/from-mesh.hpp"

struct State::Impl {
  WingedMesh        _mesh;
  Camera            _camera;
  Cursor            _cursor;
  History           _history;
  ViewMouseMovement _mouseMovement;

  WingedMesh& mesh () { return this->_mesh; }
  WingedMesh& mesh (const Id& id) {
    assert (this->_mesh.id () == id);
    return this->_mesh;
  }
  Camera&            camera        () { return this->_camera; }
  Cursor&            cursor        () { return this->_cursor; }
  History&           history       () { return this->_history; }
  ViewMouseMovement& mouseMovement () { return this->_mouseMovement; }

  void initialize () { 
    //WingedUtil :: fromMesh (this->_mesh, Mesh :: sphere (100,200));
    //WingedUtil :: fromMesh (this->_mesh, Mesh :: sphere (10,20));
    //WingedUtil :: fromMesh (this->_mesh, Mesh :: icosphere (2));
    //WingedUtil :: fromMesh (this->_mesh, Mesh :: cube ());
    this->_camera.initialize ();
    this->_cursor.initialize ();

    this->_history.add <ActionFromMesh> (this->_mesh)->icosphere (this->_mesh, 2);
  }

  void render () {
    this->_mesh.render ();
    this->_cursor.render ();
  }

  void setMesh (const Mesh&) {
    assert (false);
  }

  void writeAllData () {
    this->_mesh.write ();
  }

  void bufferAllData () {
    this->_mesh.bufferData ();
  }
};

GLOBAL               (State)
DELEGATE_CONSTRUCTOR (State)
DELEGATE_DESTRUCTOR  (State)

DELEGATE_GLOBAL  (WingedMesh&       , State, mesh)
DELEGATE1_GLOBAL (WingedMesh&       , State, mesh, const Id&)
DELEGATE_GLOBAL  (Camera&           , State, camera)
DELEGATE_GLOBAL  (Cursor&           , State, cursor)
DELEGATE_GLOBAL  (History&          , State, history)
DELEGATE_GLOBAL  (ViewMouseMovement&, State, mouseMovement)

DELEGATE_GLOBAL  (void, State, initialize)
DELEGATE_GLOBAL  (void, State, render)
DELEGATE1_GLOBAL (void, State, setMesh, const Mesh&)
DELEGATE_GLOBAL  (void, State, writeAllData)
DELEGATE_GLOBAL  (void, State, bufferAllData)

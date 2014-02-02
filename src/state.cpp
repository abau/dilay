#include "state.hpp"
#include "mesh.hpp"
#include "camera.hpp"
#include "cursor.hpp"
#include "macro.hpp"
#include "history.hpp"
#include "view/mouse-movement.hpp"
#include "id.hpp"
#include "action/new-mesh.hpp"
#include "scene.hpp"

struct State::Impl {
  Camera            _camera;
  Cursor            _cursor;
  History           _history;
  ViewMouseMovement _mouseMovement;
  Scene             _scene;

  Camera&            camera        () { return this->_camera; }
  Cursor&            cursor        () { return this->_cursor; }
  History&           history       () { return this->_history; }
  ViewMouseMovement& mouseMovement () { return this->_mouseMovement; }
  Scene&             scene         () { return this->_scene; }

  void initialize () { 
    //WingedUtil :: fromMesh (this->_mesh, Mesh :: sphere (100,200));
    //WingedUtil :: fromMesh (this->_mesh, Mesh :: sphere (10,20));
    //WingedUtil :: fromMesh (this->_mesh, Mesh :: icosphere (2));
    //WingedUtil :: fromMesh (this->_mesh, Mesh :: cube ());
    this->_camera.initialize ();
    this->_cursor.initialize ();

    this->_history.add <ActionNewMesh> ()->icosphere (2);
  }

  void render () {
    this->_scene .render ();
    this->_cursor.render ();
  }
};

GLOBAL               (State)
DELEGATE_CONSTRUCTOR (State)
DELEGATE_DESTRUCTOR  (State)

DELEGATE_GLOBAL  (Camera&           , State, camera)
DELEGATE_GLOBAL  (Cursor&           , State, cursor)
DELEGATE_GLOBAL  (History&          , State, history)
DELEGATE_GLOBAL  (ViewMouseMovement&, State, mouseMovement)
DELEGATE_GLOBAL  (Scene&            , State, scene)

DELEGATE_GLOBAL  (void, State, initialize)
DELEGATE_GLOBAL  (void, State, render)

#include "state.hpp"
#include "mesh.hpp"
#include "camera.hpp"
#include "cursor.hpp"
#include "history.hpp"
#include "view/mouse-movement.hpp"
#include "id.hpp"
#include "action/new-mesh.hpp"
#include "scene.hpp"
#include "mesh-type.hpp"

struct State::Impl {
  Camera            camera;
  Cursor            cursor;
  History           history;
  ViewMouseMovement mouseMovement;
  Scene             scene;

  void initialize () { 
    this->camera.initialize ();
    this->cursor.initialize ();
    this->history.add <ActionNewMesh> ()->icosphere (MeshType::FreeForm, 2);
  }
};

GLOBAL        (State)
DELEGATE_BIG3 (State)

GETTER_GLOBAL    (Camera&           , State, camera)
GETTER_GLOBAL    (Cursor&           , State, cursor)
GETTER_GLOBAL    (History&          , State, history)
GETTER_GLOBAL    (ViewMouseMovement&, State, mouseMovement)
GETTER_GLOBAL    (Scene&            , State, scene)
DELEGATE_GLOBAL  (void              , State, initialize)

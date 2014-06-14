#include <memory>
#include "state.hpp"
#include "mesh.hpp"
#include "camera.hpp"
#include "history.hpp"
#include "id.hpp"
#include "action/new-winged-mesh.hpp"
#include "scene.hpp"
#include "mesh-type.hpp"
#include "tool.hpp"

struct State::Impl {
  Camera                 camera;
  History                history;
  Scene                  scene;
  std::unique_ptr <Tool> toolPtr;

  void initialize () { 
    this->camera.initialize ();
    this->history.add <ActionNewWingedMesh> ()->run (MeshType::Freeform, Mesh::icosphere (2));
  }

  bool hasTool () const { 
    return this->toolPtr.get (); 
  }

  Tool& tool () { 
    assert (this->hasTool ()); 
    return *this->toolPtr; 
  }

  void setTool (Tool* tool) { 
    this->toolPtr.reset (tool); 
  }
};

GLOBAL        (State)
DELEGATE_BIG3 (State)

GETTER_GLOBAL    (Camera&           , State, camera)
GETTER_GLOBAL    (History&          , State, history)
GETTER_GLOBAL    (Scene&            , State, scene)
DELEGATE_GLOBAL  (void              , State, initialize)
DELEGATE_GLOBAL  (bool              , State, hasTool)
DELEGATE_GLOBAL  (Tool&             , State, tool)
DELEGATE1_GLOBAL (void              , State, setTool, Tool*)

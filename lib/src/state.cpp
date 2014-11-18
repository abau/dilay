#include <memory>
#include "action/new-winged-mesh.hpp"
#include "camera.hpp"
#include "history.hpp"
#include "mesh-definition.hpp"
#include "mesh-type.hpp"
#include "scene.hpp"
#include "state.hpp"
#include "tool.hpp"
#include "view/gl-widget.hpp"
#include "view/main-window.hpp"
#include "view/tool-menu-parameters.hpp"

struct State::Impl {
  Camera                 camera;
  History                history;
  Scene                  scene;
  std::unique_ptr <Tool> toolPtr;

  void initialize () { 
    this->camera.initialize ();
    this->history.add <ActionNewWingedMesh> ().run ( MeshType::Freeform
                                                   , MeshDefinition::icosphere (2));
  }

  bool hasTool () const { 
    return this->toolPtr.get (); 
  }

  Tool& tool () { 
    assert (this->hasTool ()); 
    return *this->toolPtr; 
  }

  void setTool (Tool* tool) { 
    if (tool) {
      tool->menuParameters ().mainWindow ().showMessage (tool->message ());
    }
    else if (toolPtr) {
      toolPtr->menuParameters ().mainWindow ().showDefaultMessage ();
    }
    this->toolPtr.reset (tool); 

    if (toolPtr) {
      this->handleToolResponse (toolPtr->initialize ());
    }
  }

  void handleToolResponse (ToolResponse response) {
    assert (this->hasTool ());
    switch (response) {
      case ToolResponse::None:
        break;
      case ToolResponse::Redraw:
        this->toolPtr->menuParameters ().mainWindow ().glWidget ().update ();
        break;
      case ToolResponse::Terminate:
        this->toolPtr->menuParameters ().mainWindow ().glWidget ().update ();
        this->setTool (nullptr);
        break;
    }
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
DELEGATE1_GLOBAL (void              , State, handleToolResponse, ToolResponse)

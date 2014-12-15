#include <glm/glm.hpp>
#include <memory>
#include "action/new-winged-mesh.hpp"
#include "camera.hpp"
#include "history.hpp"
#include "mesh-definition.hpp"
#include "scene.hpp"
#include "state.hpp"
#include "tool.hpp"
#include "view/gl-widget.hpp"
#include "view/main-window.hpp"
#include "view/properties/widget.hpp"
#include "view/tool/menu-parameters.hpp"
#include "util.hpp"

struct State::Impl {
  ViewMainWindow*        mainWindowPtr;
  Camera                 camera;
  History                history;
  Scene                  scene;
  std::unique_ptr <Tool> toolPtr;

  Impl () : mainWindowPtr (nullptr)
  {}

  void initialize (ViewMainWindow& mW) { 
    this->mainWindowPtr = &mW;
    this->camera.initialize ();

    MeshDefinition meshDefinition (MeshDefinition::icosphere (2));
    meshDefinition.scale          (Util::defaultScale ());

    this->history.add <ActionNewWingedMesh> ().run (meshDefinition);
  }

  ViewMainWindow& mainWindow () const {
    assert (this->mainWindowPtr);
    return *this->mainWindowPtr;
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
      tool->showToolTip ();
      this->mainWindow ().properties ().showTool (tool->menuParameters ().label ());
    }
    else if (this->toolPtr) {
      this->mainWindow ().showDefaultToolTip ();
      this->mainWindow ().properties ().resetTool ();
    }
    this->toolPtr.reset (tool); 

    if (this->toolPtr) {
      this->handleToolResponse (this->toolPtr->initialize ());
    }
  }

  void handleToolResponse (ToolResponse response) {
    assert (this->hasTool ());
    switch (response) {
      case ToolResponse::None:
        break;
      case ToolResponse::Redraw:
        this->mainWindow ().glWidget ().update ();
        break;
      case ToolResponse::Terminate:
        this->mainWindow ().glWidget ().update ();
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
DELEGATE_GLOBAL  (ViewMainWindow&   , State, mainWindow)
DELEGATE1_GLOBAL (void              , State, initialize, ViewMainWindow&)
DELEGATE_GLOBAL  (bool              , State, hasTool)
DELEGATE_GLOBAL  (Tool&             , State, tool)
DELEGATE1_GLOBAL (void              , State, setTool, Tool*)
DELEGATE1_GLOBAL (void              , State, handleToolResponse, ToolResponse)

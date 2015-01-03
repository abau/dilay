#include <glm/glm.hpp>
#include <memory>
#include "action/new-winged-mesh.hpp"
#include "camera.hpp"
#include "config.hpp"
#include "history.hpp"
#include "mesh-definition.hpp"
#include "renderer.hpp"
#include "scene.hpp"
#include "state.hpp"
#include "tool.hpp"
#include "view/gl-widget.hpp"
#include "view/main-window.hpp"
#include "view/properties/widget.hpp"
#include "view/tool/menu-parameters.hpp"
#include "util.hpp"

#include <iostream>

struct State::Impl {
  ViewMainWindow&        mainWindow;
  Config&                config;
  Renderer               renderer;
  Camera                 camera;
  History                history;
  Scene                  scene;
  std::unique_ptr <Tool> toolPtr;

  Impl (ViewMainWindow& mW, Config& c) 
    : mainWindow (mW) 
    , config     (c)
    , renderer   (config)
    , camera     (config, renderer)
    , scene      (ConfigProxy (config, "editor/poly-mesh/"))
  {
    MeshDefinition meshDefinition (MeshDefinition::icosphere (2));
    meshDefinition.scale          (Util::defaultScale ());

    this->history.add <ActionNewWingedMesh> ().run (this->scene, meshDefinition);
  }

  ~Impl () {
    this->toolPtr.reset (nullptr);
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
      this->mainWindow.properties ().showTool (tool->menuParameters ().label ());
    }
    else if (this->toolPtr) {
      this->mainWindow.showDefaultToolTip ();
      this->mainWindow.properties ().resetTool ();
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
        this->mainWindow.glWidget ().update ();
        break;
      case ToolResponse::Terminate:
        this->mainWindow.glWidget ().update ();
        this->setTool (nullptr);
        break;
    }
  }
};

DELEGATE2_BIG2 (State, ViewMainWindow&, Config&)

GETTER    (ViewMainWindow&   , State, mainWindow)
GETTER    (Config&           , State, config)
GETTER    (Renderer&         , State, renderer)
GETTER    (Camera&           , State, camera)
GETTER    (History&          , State, history)
GETTER    (Scene&            , State, scene)
DELEGATE  (bool              , State, hasTool)
DELEGATE  (Tool&             , State, tool)
DELEGATE1 (void              , State, setTool, Tool*)
DELEGATE1 (void              , State, handleToolResponse, ToolResponse)

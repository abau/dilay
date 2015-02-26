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
    MeshDefinition meshDefinition (MeshDefinition::Icosphere (2));
    meshDefinition.scale          (glm::vec3 (Util::defaultScale ()));

    this->history.add <ActionNewWingedMesh> ().run (this->scene, meshDefinition);
  }

  ~Impl () {
    this->toolPtr.reset (nullptr);
  }

  bool hasTool () const { 
    return bool (this->toolPtr); 
  }

  Tool& tool () { 
    assert (this->hasTool ()); 
    return *this->toolPtr; 
  }

  void setTool (Tool& tool) { 
    this->resetTool ();
    this->toolPtr.reset (&tool); 

    tool.showToolTip ();
    this->mainWindow.properties ().showTool (tool.menuParameters ().label ());
    this->handleToolResponse (tool.initialize ());
  }

  void resetTool () {
    if (this->hasTool ()) {
      this->toolPtr->close ();

      // order of destruction is important, because of stack-allocated widgets
      this->toolPtr.reset (); 
      this->mainWindow.showDefaultToolTip ();
      this->mainWindow.properties ().resetTool ();
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
        this->resetTool ();
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
DELEGATE1 (void              , State, setTool, Tool&)
DELEGATE  (void              , State, resetTool)
DELEGATE1 (void              , State, handleToolResponse, ToolResponse)

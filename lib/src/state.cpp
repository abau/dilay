#include <glm/glm.hpp>
#include <memory>
#include "cache.hpp"
#include "camera.hpp"
#include "config.hpp"
#include "history.hpp"
#include "mesh-definition.hpp"
#include "mesh.hpp"
#include "scene.hpp"
#include "state.hpp"
#include "tool.hpp"
#include "view/gl-widget.hpp"
#include "view/main-window.hpp"
#include "view/properties.hpp"
#include "winged/util.hpp"

struct State::Impl {
  ViewMainWindow&        mainWindow;
  Config&                config;
  Cache&                 cache;
  Camera                 camera;
  History                history;
  Scene                  scene;
  std::unique_ptr <Tool> toolPtr;

  Impl (ViewMainWindow& mW, Config& cfg, Cache& cch) 
    : mainWindow (mW) 
    , config     (cfg)
    , cache      (cch)
    , camera     (this->config)
    , scene      (this->config)
  {
    WingedUtil::defaultScale (this->scene.newWingedMesh (MeshDefinition::icosphere (3)));
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

  void setTool (Tool&& tool) { 
    assert (this->toolPtr == false);

    this->toolPtr.reset (&tool); 

    ToolResponse initResponse = tool.initialize ();
    switch (initResponse) {
      case ToolResponse::None:
        this->handleToolResponse (ToolResponse::Redraw);
        break;
      default:
        this->handleToolResponse (initResponse);
        break;
    }
  }

  void resetTool (bool deselect) {
    if (this->hasTool ()) {
      this->toolPtr->close ();

      // order of destruction is important, because of stack-allocated widgets
      this->toolPtr.reset (); 
      this->mainWindow.showDefaultToolTip ();
      this->mainWindow.properties ().reset ();

      if (deselect) {
        this->mainWindow.deselectTool ();
      }
      this->mainWindow.update ();
    }
  }

  void fromConfig () {
    this->camera.fromConfig (this->config);
    this->scene .fromConfig (this->config);
  }

  void handleToolResponse (ToolResponse response) {
    assert (this->hasTool ());
    switch (response) {
      case ToolResponse::None:
        break;
      case ToolResponse::Redraw:
        this->mainWindow.update ();
        break;
      case ToolResponse::Terminate:
        this->resetTool (true);
        break;
    }
  }
};

DELEGATE3_BIG2 (State, ViewMainWindow&, Config&, Cache&)

GETTER    (ViewMainWindow&   , State, mainWindow)
GETTER    (Config&           , State, config)
GETTER    (Cache&            , State, cache)
GETTER    (Camera&           , State, camera)
GETTER    (History&          , State, history)
GETTER    (Scene&            , State, scene)
DELEGATE  (bool              , State, hasTool)
DELEGATE  (Tool&             , State, tool)
DELEGATE1 (void              , State, setTool, Tool&&)
DELEGATE1 (void              , State, resetTool, bool)
DELEGATE  (void              , State, fromConfig)
DELEGATE1 (void              , State, handleToolResponse, ToolResponse)

/* This file is part of Dilay
 * Copyright © 2015,2016 Alexander Bau
 * Use and redistribute under the terms of the GNU General Public License
 */
#include <memory>
#include "cache.hpp"
#include "camera.hpp"
#include "config.hpp"
#include "history.hpp"
#include "mesh.hpp"
#include "mesh-util.hpp"
#include "scene.hpp"
#include "state.hpp"
#include "tool.hpp"
#include "view/gl-widget.hpp"
#include "view/main-window.hpp"
#include "view/tool-pane.hpp"
#include "view/two-column-grid.hpp"

struct State::Impl {
  State*                 self;
  ViewMainWindow&        mainWindow;
  Config&                config;
  Cache&                 cache;
  Camera                 camera;
  History                history;
  Scene                  scene;
  std::unique_ptr <Tool> toolPtr;

  Impl (State* s, ViewMainWindow& mW, Config& cfg, Cache& cch) 
    : self       (s)
    , mainWindow (mW) 
    , config     (cfg)
    , cache      (cch)
    , camera     (this->config)
    , history    (this->config)
    , scene      (this->config)
  {
    this->scene.newDynamicMesh (this->config, MeshUtil::icosphere (4));
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
    assert (this->hasTool () == false);

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

      this->toolPtr.reset (); 
      this->mainWindow.showDefaultToolTip ();
      this->mainWindow.toolPane ().resetProperties ();

      if (deselect) {
        this->mainWindow.toolPane ().deselectTool ();
      }
      this->mainWindow.update ();
    }
  }

  void fromConfig () {
    this->camera .fromConfig (this->config);
    this->history.fromConfig (this->config);
    this->scene  .fromConfig (this->config);

    if (this->hasTool ()) {
      this->toolPtr->fromConfig ();
    }
  }

  void undo () {
    this->history.undo (*this->self);
    this->mainWindow.update ();
  }

  void redo () {
    this->history.redo (*this->self);
    this->mainWindow.update ();
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

DELEGATE3_BIG2_SELF (State, ViewMainWindow&, Config&, Cache&)

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
DELEGATE  (void              , State, undo)
DELEGATE  (void              , State, redo)
DELEGATE1 (void              , State, handleToolResponse, ToolResponse)

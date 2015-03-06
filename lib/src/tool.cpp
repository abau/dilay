#include <QMouseEvent>
#include <QPushButton>
#include <glm/glm.hpp>
#include "action.hpp"
#include "cache.hpp"
#include "config.hpp"
#include "history.hpp"
#include "scene.hpp"
#include "state.hpp"
#include "tool.hpp"
#include "view/gl-widget.hpp"
#include "view/main-window.hpp"
#include "view/properties.hpp"
#include "view/properties/widget.hpp"
#include "view/tool/menu-parameters.hpp"
#include "view/tool/tip.hpp"
#include "view/util.hpp"

struct Tool::Impl {
  Tool*                  self;
  ViewToolMenuParameters menuParameters;
  ViewToolTip            toolTip;
  Config&                config;
  CacheProxy             cache;
  const Action*          undoLimit;

  Impl (Tool* s, const ViewToolMenuParameters& p, const char* key) 
    : self           (s) 
    , menuParameters (p)
    , config         (p.state ().config ())
    , cache          (p.state ().cache (), "editor/tool/" + std::string (key) + "/")
    , undoLimit      (p.state ().history ().recent ())
  {
    QPushButton& close = ViewUtil::pushButton (QObject::tr ("Close"));
    this->properties ().footer ().add (close);

    QObject::connect (&close, &QPushButton::clicked, [this] () {
      this->state ().resetTool ();
    });
    this->resetToolTip ();
  }

  void showToolTip () {
    this->state ().mainWindow ().showToolTip (this->toolTip);
  }

  bool allowUndo () const {
    return ( this->menuParameters.state ().history ().recent () != this->undoLimit )
        && ( this->self->runAllowUndoRedo () );
  }

  bool allowRedo () const {
    return this->self->runAllowUndoRedo ();
  }

  ToolResponse initialize () { 
    return this->self->runInitialize ();
  }

  void render () const { 
    return this->self->runRender ( ); 
  }

  ToolResponse mouseMoveEvent (const QMouseEvent& e) { 
    return this->self->runMouseMoveEvent (e);
  }

  ToolResponse mousePressEvent (const QMouseEvent& e) {
    if (e.button () == Qt::RightButton) {
      return ToolResponse::None;
    }
    else {
      return this->self->runMousePressEvent (e);
    }
  }

  ToolResponse mouseReleaseEvent (const QMouseEvent& e) {
    if (e.button () == Qt::RightButton) {
      return ToolResponse::Terminate;
    }
    else {
      return this->self->runMouseReleaseEvent (e);
    }
  }

  ToolResponse wheelEvent (const QWheelEvent& e) {
    return this->self->runWheelEvent (e);
  }

  void close () { 
    return this->self->runClose (); 
  }

  State& state () const {
    return this->menuParameters.state ();
  }

  void updateGlWidget () {
    this->state ().mainWindow ().glWidget ().update ();
  }

  ViewProperties& properties () const {
    return this->state ().mainWindow ().properties ().tool ();
  }

  void resetToolTip () {
    this->toolTip.reset ();
    this->toolTip.add   (ViewToolTip::MouseEvent::Right, QObject::tr ("Close"));
  }

  glm::ivec2 cursorPosition () {
    return this->state ().mainWindow ().glWidget ().cursorPosition ();
  }
};

DELEGATE2_BIG3_SELF (Tool, const ViewToolMenuParameters&, const char*)
GETTER_CONST   (const ViewToolMenuParameters&, Tool, menuParameters)
DELEGATE       (void                         , Tool, showToolTip)
DELEGATE_CONST (bool                         , Tool, allowUndo)
DELEGATE_CONST (bool                         , Tool, allowRedo)
DELEGATE       (ToolResponse                 , Tool, initialize)
DELEGATE_CONST (void                         , Tool, render)
DELEGATE1      (ToolResponse                 , Tool, mouseMoveEvent, const QMouseEvent&)
DELEGATE1      (ToolResponse                 , Tool, mousePressEvent, const QMouseEvent&)
DELEGATE1      (ToolResponse                 , Tool, mouseReleaseEvent, const QMouseEvent&)
DELEGATE1      (ToolResponse                 , Tool, wheelEvent, const QWheelEvent&)
DELEGATE       (void                         , Tool, close)
DELEGATE_CONST (State&                       , Tool, state)
DELEGATE       (void                         , Tool, updateGlWidget)
DELEGATE_CONST (ViewProperties&              , Tool, properties)
GETTER_CONST   (ViewToolTip&                 , Tool, toolTip)
DELEGATE       (void                         , Tool, resetToolTip)
GETTER_CONST   (Config&                      , Tool, config)
GETTER_CONST   (CacheProxy&                  , Tool, cache)
DELEGATE_CONST (glm::ivec2                   , Tool, cursorPosition)

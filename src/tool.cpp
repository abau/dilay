#include "tool.hpp"
#include "view/tool-options.hpp"
#include "view/main-window.hpp"
#include "view/gl-widget.hpp"
#include "state.hpp"

struct Tool::Impl {
  Tool*              self;
  ViewMainWindow*    mainWindow;
  QContextMenuEvent* menuEvent;
  ViewToolOptions*   toolOptions;

  Impl () {}

  Impl (Tool* s, ViewMainWindow* w, QContextMenuEvent* e, const QString& name) 
    : self        (s) 
    , mainWindow  (w)
    , menuEvent   (e)
    , toolOptions (new ViewToolOptions (w))
  {
    this->toolOptions->setWindowTitle (name);

    QObject::connect ( this->toolOptions, &ViewToolOptions::accepted
                     , [this] () { State::setTool (nullptr); } );

    QObject::connect ( this->toolOptions, &ViewToolOptions::rejected
                     , [this] () { State::setTool (nullptr); } );

    this->toolOptions->show ();
  }

  ~Impl () {
    this->toolOptions->close ();
  }

  void render () { 
    return this->self->runRender ( ); 
  }

  void mouseMoveEvent (QMouseEvent* e) { 
    if (this->self->runMouseMoveEvent (e)) {
      this->mainWindow->glWidget ()->update ();
    }
  }

  void mousePressEvent (QMouseEvent* e) {
    if (this->self->runMousePressEvent (e)) {
      this->mainWindow->glWidget ()->update ();
    }
  }
};

DELEGATE3_BIG3_SELF  (Tool, ViewMainWindow*, QContextMenuEvent*, const QString&)
DELEGATE_CONSTRUCTOR (Tool)
GETTER    (ViewMainWindow*   , Tool, mainWindow)
GETTER    (QContextMenuEvent*, Tool, menuEvent)
GETTER    (ViewToolOptions*  , Tool, toolOptions)
DELEGATE  (void              , Tool, render)
DELEGATE1 (void              , Tool, mouseMoveEvent, QMouseEvent*)
DELEGATE1 (void              , Tool, mousePressEvent, QMouseEvent*)

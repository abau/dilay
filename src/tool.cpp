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
  bool               _isDraged;

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
    this->isDraged (false);
    this->toolOptions->close ();
  }

  void render () { 
    return this->self->runRender ( ); 
  }

  bool isDraged () const { 
    return this->_isDraged;
  }

  void isDraged (bool b) { 
    this->_isDraged = b;

    if (this->_isDraged) {
      this->self->mainWindow ()->glWidget ()->setCursor (QCursor (Qt::SizeAllCursor));
    }
    else {
      this->self->mainWindow ()->glWidget ()->unsetCursor ();
    }
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

  void mouseReleaseEvent (QMouseEvent* e) {
    if (this->self->runMouseReleaseEvent (e)) {
      this->mainWindow->glWidget ()->update ();
    }
  }
};

DELEGATE3_BIG3_SELF  (Tool, ViewMainWindow*, QContextMenuEvent*, const QString&)
DELEGATE_CONSTRUCTOR (Tool)
GETTER         (ViewMainWindow*   , Tool, mainWindow)
GETTER         (QContextMenuEvent*, Tool, menuEvent)
GETTER         (ViewToolOptions*  , Tool, toolOptions)
DELEGATE       (void              , Tool, render)
DELEGATE_CONST (bool              , Tool, isDraged)
DELEGATE1      (void              , Tool, isDraged, bool)
DELEGATE1      (void              , Tool, mouseMoveEvent, QMouseEvent*)
DELEGATE1      (void              , Tool, mousePressEvent, QMouseEvent*)
DELEGATE1      (void              , Tool, mouseReleaseEvent, QMouseEvent*)

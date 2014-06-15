#include <glm/glm.hpp>
#include "tool.hpp"
#include "view/tool-options.hpp"
#include "view/main-window.hpp"
#include "view/gl-widget.hpp"
#include "state.hpp"
#include "scene.hpp"

struct Tool::Impl {
  Tool*              self;
  ViewMainWindow&    mainWindow;
  glm::ivec2         clickPosition;
  ViewToolOptions*   toolOptions;

  Impl (Tool* s, ViewMainWindow& w, const glm::ivec2& p, const QString& name) 
    : Impl (s,w,p)
  {
    this->toolOptions = new ViewToolOptions (w);
    this->toolOptions->setWindowTitle (name);

    QObject::connect ( this->toolOptions, &ViewToolOptions::accepted
                     , [this] () { State::setTool (nullptr); } );

    QObject::connect ( this->toolOptions, &ViewToolOptions::rejected
                     , [this] () { State::setTool (nullptr); } );

    this->toolOptions->show ();
  }

  Impl (Tool* s, ViewMainWindow& w, const glm::ivec2& p) 
    : self          (s) 
    , mainWindow    (w)
    , clickPosition (p)
    , toolOptions   (nullptr)
  {}

  ~Impl () {
    if (this->toolOptions) {
      this->toolOptions->close  ();
    }
  }

  void render () { 
    return this->self->runRender ( ); 
  }

  void mouseMoveEvent (QMouseEvent& e) { 
    if (this->self->runMouseMoveEvent (e)) {
      this->updateGlWidget ();
    }
  }

  void mousePressEvent (QMouseEvent& e) {
    if (this->self->runMousePressEvent (e)) {
      this->updateGlWidget ();
    }
  }

  void mouseReleaseEvent (QMouseEvent& e) {
    if (this->self->runMouseReleaseEvent (e)) {
      this->updateGlWidget ();
    }
  }

  void wheelEvent (QWheelEvent& e) {
    if (this->self->runWheelEvent (e)) {
      this->updateGlWidget ();
    }
  }

  QString message () const { return this->self->runMessage (); }

  void updateGlWidget () {
    this->mainWindow.glWidget ().update ();
  }

  void unselectAll () {
    if (State::scene ().unselectAll ()) {
      this->updateGlWidget ();
    }
  }

  void selectIntersection () {
    this->mainWindow.glWidget ().selectIntersection ();
  }

  void selectIntersection (const glm::ivec2& pos) {
    this->mainWindow.glWidget ().selectIntersection (pos);
  }
};

DELEGATE3_BIG3_SELF        (Tool, ViewMainWindow&, const glm::ivec2&, const QString&)
DELEGATE2_CONSTRUCTOR_SELF (Tool, ViewMainWindow&, const glm::ivec2&) 
GETTER         (ViewMainWindow&   , Tool, mainWindow)
GETTER_CONST   (const glm::ivec2& , Tool, clickPosition)
DELEGATE       (void              , Tool, updateGlWidget)
DELEGATE       (void              , Tool, unselectAll)
DELEGATE       (void              , Tool, selectIntersection)
DELEGATE1      (void              , Tool, selectIntersection, const glm::ivec2&)
DELEGATE       (void              , Tool, render)
DELEGATE1      (void              , Tool, mouseMoveEvent, QMouseEvent&)
DELEGATE1      (void              , Tool, mousePressEvent, QMouseEvent&)
DELEGATE1      (void              , Tool, mouseReleaseEvent, QMouseEvent&)
DELEGATE1      (void              , Tool, wheelEvent, QWheelEvent&)
DELEGATE_CONST (QString           , Tool, message)
GETTER         (ViewToolOptions*  , Tool, toolOptions)

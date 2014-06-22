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

  ToolResponse mouseMoveEvent (QMouseEvent& e) { 
    return this->self->runMouseMoveEvent (e);
  }

  ToolResponse mousePressEvent (QMouseEvent& e) {
    return this->self->runMousePressEvent (e);
  }

  ToolResponse mouseReleaseEvent (QMouseEvent& e) {
    return this->self->runMouseReleaseEvent (e);
  }

  ToolResponse wheelEvent (QWheelEvent& e) {
    return this->self->runWheelEvent (e);
  }

  QString message () const { 
    return this->self->runMessage (); 
  }
};

DELEGATE3_BIG3_SELF        (Tool, ViewMainWindow&, const glm::ivec2&, const QString&)
DELEGATE2_CONSTRUCTOR_SELF (Tool, ViewMainWindow&, const glm::ivec2&) 
GETTER         (ViewMainWindow&   , Tool, mainWindow)
GETTER_CONST   (const glm::ivec2& , Tool, clickPosition)
DELEGATE       (void              , Tool, render)
DELEGATE1      (ToolResponse      , Tool, mouseMoveEvent, QMouseEvent&)
DELEGATE1      (ToolResponse      , Tool, mousePressEvent, QMouseEvent&)
DELEGATE1      (ToolResponse      , Tool, mouseReleaseEvent, QMouseEvent&)
DELEGATE1      (ToolResponse      , Tool, wheelEvent, QWheelEvent&)
DELEGATE_CONST (QString           , Tool, message)
GETTER         (ViewToolOptions*  , Tool, toolOptions)

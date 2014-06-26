#include <glm/glm.hpp>
#include "tool.hpp"
#include "view/tool-parameters.hpp"
#include "view/main-window.hpp"
#include "view/gl-widget.hpp"
#include "state.hpp"
#include "scene.hpp"

struct Tool::Impl {
  Tool*               self;
  ViewMainWindow&     mainWindow;
  glm::ivec2          clickPosition;
  ViewToolParameters* toolParameters;

  Impl (Tool* s, ViewMainWindow& w, const glm::ivec2& p, const QString& name) 
    : Impl (s,w,p)
  {
    this->toolParameters = new ViewToolParameters (w);
    this->toolParameters->setWindowTitle (name);

    QObject::connect ( this->toolParameters, &ViewToolParameters::accepted
                     , [this] () { State::setTool (nullptr); } );

    QObject::connect ( this->toolParameters, &ViewToolParameters::rejected
                     , [this] () { State::setTool (nullptr); } );

    this->toolParameters->show ();
  }

  Impl (Tool* s, ViewMainWindow& w, const glm::ivec2& p) 
    : self           (s) 
    , mainWindow     (w)
    , clickPosition  (p)
    , toolParameters (nullptr)
  {}

  ~Impl () {
    if (this->toolParameters) {
      this->toolParameters->close  ();
    }
  }

  ToolResponse execute () { 
    return this->self->runExecute ();
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
GETTER         (ViewMainWindow&    , Tool, mainWindow)
GETTER_CONST   (const glm::ivec2&  , Tool, clickPosition)
DELEGATE       (ToolResponse       , Tool, execute)
DELEGATE       (void               , Tool, render)
DELEGATE1      (ToolResponse       , Tool, mouseMoveEvent, QMouseEvent&)
DELEGATE1      (ToolResponse       , Tool, mousePressEvent, QMouseEvent&)
DELEGATE1      (ToolResponse       , Tool, mouseReleaseEvent, QMouseEvent&)
DELEGATE1      (ToolResponse       , Tool, wheelEvent, QWheelEvent&)
DELEGATE_CONST (QString            , Tool, message)
GETTER         (ViewToolParameters*, Tool, toolParameters)

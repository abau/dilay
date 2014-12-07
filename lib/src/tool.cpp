#include <QMouseEvent>
#include <QPushButton>
#include <glm/glm.hpp>
#include "scene.hpp"
#include "state.hpp"
#include "tool.hpp"
#include "view/gl-widget.hpp"
#include "view/main-window.hpp"
#include "view/properties.hpp"
#include "view/properties/widget.hpp"
#include "view/tool-menu-parameters.hpp"
#include "view/util.hpp"

struct Tool::Impl {
  Tool*                  self;
  const bool             hasProperties;
  ViewToolMenuParameters menuParameters;

  Impl (Tool* s, const ViewToolMenuParameters& p, const QString& name) 
    : self           (s) 
    , hasProperties  (true)
    , menuParameters (p)
  {
    this->menuParameters.mainWindow ().properties ().showTool (name);

    QPushButton& apply = ViewUtil::pushButton (QObject::tr ("Apply"), true, true);
    this->properties ().setFooter (apply);

    QObject::connect (&apply, &QPushButton::clicked, [this] () {
      this->close ();
      State::setTool (nullptr);
    });
  }

  Impl (Tool* s, const ViewToolMenuParameters& p) 
    : self           (s) 
    , hasProperties  (false)
    , menuParameters (p)
  {}

  ~Impl () {
    if (this->hasProperties) {
      this->menuParameters.mainWindow ().properties ().resetTool ();
    }
  }

  ToolResponse initialize () { 
    return this->self->runInitialize ();
  }

  void render () { 
    return this->self->runRender ( ); 
  }

  ToolResponse mouseMoveEvent (QMouseEvent& e) { 
    return this->self->runMouseMoveEvent (e);
  }

  ToolResponse mouseReleaseEvent (QMouseEvent& e) {
    if (e.button () == Qt::RightButton) {
      this->close ();
      return ToolResponse::Terminate;
    }
    else {
      return this->self->runMouseReleaseEvent (e);
    }
  }

  ToolResponse wheelEvent (QWheelEvent& e) {
    return this->self->runWheelEvent (e);
  }

  QString message () const { 
    return this->self->runMessage (); 
  }

  void close () { 
    return this->self->runClose (); 
  }

  void updateGlWidget () {
    this->menuParameters.mainWindow ().glWidget ().update ();
  }

  ViewProperties& properties () {
    assert (this->hasProperties);
    return this->menuParameters.mainWindow ().properties ().tool ();
  }
};

DELEGATE2_BIG3_SELF        (Tool, const ViewToolMenuParameters&, const QString&)
DELEGATE1_CONSTRUCTOR_SELF (Tool, const ViewToolMenuParameters&) 
GETTER_CONST   (const ViewToolMenuParameters&, Tool, menuParameters)
DELEGATE       (ToolResponse                 , Tool, initialize)
DELEGATE       (void                         , Tool, render)
DELEGATE1      (ToolResponse                 , Tool, mouseMoveEvent, QMouseEvent&)
DELEGATE1      (ToolResponse                 , Tool, mouseReleaseEvent, QMouseEvent&)
DELEGATE1      (ToolResponse                 , Tool, wheelEvent, QWheelEvent&)
DELEGATE_CONST (QString                      , Tool, message)
DELEGATE       (void                         , Tool, close)
DELEGATE       (void                         , Tool, updateGlWidget)
DELEGATE       (ViewProperties&              , Tool, properties)

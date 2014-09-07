#include <QMouseEvent>
#include <glm/glm.hpp>
#include "scene.hpp"
#include "state.hpp"
#include "tool.hpp"
#include "view/gl-widget.hpp"
#include "view/main-window.hpp"
#include "view/tool-menu-parameters.hpp"
#include "view/tool-parameters.hpp"

struct Tool::Impl {
  Tool*                  self;
  ViewToolMenuParameters menuParameters;
  ViewToolParameters*    toolParameters;

  Impl (Tool* s, const ViewToolMenuParameters& p, const QString& name) 
    : Impl (s,p)
  {
    const bool alwaysOpen = this->menuParameters.rightClick ();

    this->toolParameters = new ViewToolParameters (p.mainWindow (), alwaysOpen);
    this->toolParameters->setWindowTitle (name);

    QObject::connect ( this->toolParameters, &ViewToolParameters::finished
                     , [this] (int r) { 
                         if (r == ViewToolParameters::Result::Apply) {
                           this->toolParameters->hide ();
                         }
                         else if (r == ViewToolParameters::Result::ApplyAndClose) {
                           this->close    ();
                           State::setTool (nullptr); 
                         }
                         else if (r == ViewToolParameters::Result::Cancel) {
                           this->cancel   ();
                           State::setTool (nullptr); 
                         }
                         else { assert (false); }
                       }
                     );

    QObject::connect ( this->toolParameters, &ViewToolParameters::rejected
                     , [this] () { State::setTool (nullptr); } );

    if (alwaysOpen) {
      this->toolParameters->show ();
    }
  }

  Impl (Tool* s, const ViewToolMenuParameters& p) 
    : self           (s) 
    , menuParameters (p)
    , toolParameters (nullptr)
  {}

  ~Impl () {
    if (this->toolParameters) {
      this->toolParameters->close  ();
    }
  }

  ToolResponse initialize () { 
    return this->self->runInitialize ();
  }

  void render () { 
    return this->self->runRender ( ); 
  }

  ToolResponse mouseMoveEvent (QMouseEvent& e) { 
    if (this->toolParameters && this->toolParameters->isVisible ()) {
      return ToolResponse::None;
    }
    else {
      return this->self->runMouseMoveEvent (e);
    }
  }

  ToolResponse mouseReleaseEvent (QMouseEvent& e) {
    if (this->toolParameters && this->toolParameters->isVisible ()) {
      return ToolResponse::None;
    }
    else if (e.button () == Qt::RightButton) {
      this->cancel ();
      return ToolResponse::Terminate;
    }
    else {
      return this->self->runMouseReleaseEvent (e);
    }
  }

  ToolResponse wheelEvent (QWheelEvent& e) {
    if (this->toolParameters && this->toolParameters->isVisible ()) {
      return ToolResponse::None;
    }
    else {
      return this->self->runWheelEvent (e);
    }
  }

  QString message () const { 
    return this->self->runMessage (); 
  }

  void close () { 
    return this->self->runClose (); 
  }

  void cancel () { 
    return this->self->runCancel (); 
  }

  void updateGlWidget () {
    this->menuParameters.mainWindow ().glWidget ().update ();
  }

  ToolResponse closeOrCancelOnClick (QMouseEvent& e) {
    if (e.button () == Qt::LeftButton) {
      this->close ();
      return ToolResponse::Terminate;
    }
    else if (e.button () == Qt::RightButton) {
      this->cancel ();
      return ToolResponse::Terminate;
    }
    return ToolResponse::None;
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
DELEGATE       (void                         , Tool, cancel)
GETTER         (ViewToolParameters*          , Tool, toolParameters)
DELEGATE       (void                         , Tool, updateGlWidget)
DELEGATE1      (ToolResponse                 , Tool, closeOrCancelOnClick, QMouseEvent&)

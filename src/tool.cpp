#include <QPushButton>
#include "tool.hpp"
#include "tool/options.hpp"
#include "view/main-window.hpp"
#include "view/gl-widget.hpp"
#include "state.hpp"

struct Tool::Impl {
  Tool*           self;
  QString         toolName;
  ViewMainWindow* mainWindow;
  ToolOptions*    toolOptions;

  Impl (Tool* s) 
    : self        (s) 
    , mainWindow  (nullptr)
    , toolOptions (nullptr)
  {}

  void initialize (ViewMainWindow* w, QContextMenuEvent* e) {
    this->mainWindow = w;
    this->toolOptions = new ToolOptions (w);
    this->toolOptions->setWindowTitle   (this->toolName);

    QObject::connect ( this->toolOptions->applyButton (), &QPushButton::released
                     , [this] () { this->apply (); } );

    QObject::connect ( this->toolOptions->cancelButton (), &QPushButton::released
                     , [this] () { this->close (); } );

    this->self->runInitialize (e);
    this->toolOptions->show ();
  }

  void render () { 
    return this->self->runRender ( ); 
  }

  void mouseMoveEvent (QMouseEvent* e) { 
    this->self->runMouseMoveEvent (e); 
  }

  void mousePressEvent (QMouseEvent* e) {
    this->self->runMousePressEvent (e);
  }

  void apply () {
    this->self->runApply ();
    this->close          ();
  }

  void close () {
    assert (this->toolOptions);
    this->self->runClose     ();
    this->toolOptions->close ();
    State::setTool (nullptr);
  }
};

DELEGATE_BIG3_SELF (Tool)
GETTER    (const QString& , Tool, toolName)
SETTER    (const QString& , Tool, toolName)
GETTER    (ViewMainWindow*, Tool, mainWindow)
GETTER    (ToolOptions*   , Tool, toolOptions)
DELEGATE2 (void           , Tool, initialize, ViewMainWindow*, QContextMenuEvent*)
DELEGATE  (void           , Tool, render)
DELEGATE1 (void           , Tool, mouseMoveEvent, QMouseEvent*)
DELEGATE1 (void           , Tool, mousePressEvent, QMouseEvent*)
DELEGATE  (void           , Tool, close)

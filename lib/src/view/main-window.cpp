#include <QStatusBar>
#include "view/main-window.hpp"
#include "view/main-widget.hpp"
#include "view/tool-message.hpp"

struct ViewMainWindow :: Impl {
  ViewMainWindow* self;
  ViewMainWidget& mainWidget;
  QStatusBar&     statusBar;

  Impl (ViewMainWindow* s) 
    : self       (s) 
    , mainWidget (*new ViewMainWidget (*this->self))
    , statusBar  (*new QStatusBar     ())
  {
    this->self->setCentralWidget       (&this->mainWidget);
    this->self->setStatusBar           (&this->statusBar);
    this->statusBar.setSizeGripEnabled (false);
  }

  ViewGlWidget&         glWidget   () { return this->mainWidget.glWidget   (); }
  ViewPropertiesWidget& properties () { return this->mainWidget.properties (); }

  void showMessage (const QString& message) {
    this->statusBar.showMessage (message);
  }

  void showMessage (const std::initializer_list <ViewToolMessage>& messages) {
    this->showMessage (ViewToolMessage::message (messages));
  }
};

DELEGATE_BIG3_SELF (ViewMainWindow)
DELEGATE  (ViewGlWidget&        , ViewMainWindow, glWidget)
DELEGATE  (ViewPropertiesWidget&, ViewMainWindow, properties)
DELEGATE1 (void                 , ViewMainWindow, showMessage, const QString&)
DELEGATE1 (void                 , ViewMainWindow, showMessage, const std::initializer_list<ViewToolMessage>&)

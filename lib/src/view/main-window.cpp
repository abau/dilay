#include <QToolBar>
#include <QLabel>
#include "view/main-window.hpp"
#include "view/main-widget.hpp"
#include "view/tool-message.hpp"

struct ViewMainWindow :: Impl {
  ViewMainWindow* self;
  ViewMainWidget& mainWidget;
  QToolBar&       statusBar;
  QLabel&         messageLabel;

  Impl (ViewMainWindow* s) 
    : self         (s) 
    , mainWidget   (*new ViewMainWidget (*this->self))
    , statusBar    (*new QToolBar       ())
    , messageLabel (*new QLabel         ())
  {
    this->self->setCentralWidget       (&this->mainWidget);
    this->self->addToolBar             (Qt::BottomToolBarArea, &this->statusBar);
    this->statusBar.setFloatable       (false);
    this->statusBar.setMovable         (false);
    this->statusBar.addWidget          (&this->messageLabel);
    this->showDefaultMessage           ();
  }

  ViewGlWidget&         glWidget   () { return this->mainWidget.glWidget   (); }
  ViewPropertiesWidget& properties () { return this->mainWidget.properties (); }

  void showMessage (const QString& message) {
    this->messageLabel.setText (message);
  }

  void showDefaultMessage () {
    this->showMessage (ViewToolMessage::message 
        ({ ViewToolMessage (QObject::tr ("Select"))     .left   ()
         , ViewToolMessage (QObject::tr ("Camera Mode")).middle ()
         , ViewToolMessage (QObject::tr ("Menu"))       .right  ()
         }));
  }
};

DELEGATE_BIG3_SELF (ViewMainWindow)
DELEGATE  (ViewGlWidget&        , ViewMainWindow, glWidget)
DELEGATE  (ViewPropertiesWidget&, ViewMainWindow, properties)
DELEGATE1 (void                 , ViewMainWindow, showMessage, const QString&)
DELEGATE  (void                 , ViewMainWindow, showDefaultMessage)
